#include <arch/paging.h>
#include <stdlib.h>
#include <types/base.h>

#include "assert.h"
#include "util.h"

/*
 * An auxillary malloc used for small kernel object allocations.
 * Larger objects should be handled by the root server.
 */

#define NO_CLASSES             9
#define MIN_BLOCK_SIZE         sizeof(vaddr_t)
#define BAG_FREELIST_SENTINEL  -1
#define PAGE_FREELIST_SENTINEL 0

// TODO: track no of alloc'd objects and return pages to the pool when free

typedef struct page_hdr {
    struct page_hdr* next;
    unsigned         object_size : 16;
    signed           freelist_head_offset : 16;
} page_hdr_t;

typedef struct bag {
    page_hdr_t* free;
    page_hdr_t* full;
} bag_t;

#define OBJS_PER_PAGE(obj_type) ((ARCH_PAGE_SIZE - sizeof(page_hdr_t)) / sizeof(obj_type))

static vaddr_t pool_start      = 0;
static vaddr_t pool_end        = 0;
static paddr_t pool_start_phys = 0;

static vaddr_t page_freelist_head = 0;
static vaddr_t page_freelist_tail = 0;

static bag_t bags[NO_CLASSES] = { 0 };

static inline void free_page(vaddr_t page)
{
    ASSERT_MSG(page < pool_start && page >= pool_end, "Invalid page freed");

    *((vaddr_t*)page_freelist_tail) = page;
}

static inline vaddr_t fetch_page(void)
{
    if (page_freelist_head == 0) {
        panic("Kmalloc: No free pages left");
        return 0;
    }

    vaddr_t ret        = page_freelist_head;
    page_freelist_head = *((vaddr_t*)page_freelist_head);

    return ret;
}

static inline void init_page(vaddr_t page, size_t obj_size)
{
    int     i = 0;
    vaddr_t addr;
    for (addr = page + sizeof(page_hdr_t); addr < (page + PAGE_SIZE); addr += obj_size, i++) {
        *(int*)addr = i + 1;
    }

    *(int*)(addr - obj_size) = BAG_FREELIST_SENTINEL;
}

static inline void* freelist_dequeue(page_hdr_t* hdr, size_t obj_size)
{
    void* ret                 = (void*)(((uint8_t*)hdr + sizeof(page_hdr_t)) + (hdr->freelist_head_offset * obj_size));
    hdr->freelist_head_offset = *(int32_t*)ret;

    return ret;
}

static inline void freelist_enqueue(page_hdr_t* hdr, void* ptr)
{
    uint8_t* objs_start = (uint8_t*)hdr + sizeof(page_hdr_t);
    int32_t  offset     = ((uint8_t*)ptr - objs_start) / hdr->object_size;

    *((int32_t*)ptr)          = hdr->freelist_head_offset;
    hdr->freelist_head_offset = offset;
}

static inline void initialize_bag(size_t idx)
{
    bag_t*      bag          = &bags[idx];
    vaddr_t     bag_page     = fetch_page();
    page_hdr_t* root_hdr     = (page_hdr_t*)bag_page;
    size_t      bag_obj_size = 1 << idx;

    root_hdr->next                 = NULL;
    root_hdr->freelist_head_offset = 0;
    root_hdr->object_size          = bag_obj_size;

    init_page(bag_page, bag_obj_size);

    bag->free = root_hdr;
    bag->full = NULL;
}

static inline void* fetch_object(size_t idx)
{
    bag_t* bag         = &bags[idx];
    size_t object_size = 1 << idx;
    void*  ret         = NULL;

    if (bag->free == NULL && bag->full == NULL) {
        initialize_bag(idx);
        ret = freelist_dequeue(bag->free, object_size);
        return ret;
    }

    ret = freelist_dequeue(bag->free, object_size);
    if (bag->free->freelist_head_offset == BAG_FREELIST_SENTINEL) {
        page_hdr_t* tmp_hdr = bag->free;

        bag->free     = tmp_hdr->next;
        tmp_hdr->next = bag->full;
        bag->full     = tmp_hdr;

        if (bag->free == NULL) {
            vaddr_t     new_page = fetch_page();
            page_hdr_t* new_hdr  = (page_hdr_t*)new_page;

            new_hdr->next                 = NULL;
            new_hdr->object_size          = object_size;
            new_hdr->freelist_head_offset = 0;

            init_page(new_page, object_size);
        }
    }

    return ret;
}

void kmalloc_init(uint8_t* pool_vaddr, uint8_t* pool_phys, size_t size)
{
    ASSERT_MSG(((vaddr_t)pool_vaddr & ~PAGE_MASK) == 0, "Pool start not page-aligned");

    pool_start      = (vaddr_t)pool_vaddr;
    pool_end        = (vaddr_t)(pool_vaddr + size);
    pool_start_phys = (paddr_t)pool_phys;

    for (vaddr_t addr = pool_start; addr < pool_end; addr += PAGE_SIZE) {
        *((vaddr_t*)addr) = addr + PAGE_SIZE;
    }

    *((vaddr_t*)(pool_end - PAGE_SIZE)) = PAGE_FREELIST_SENTINEL;
}

void* kmalloc(size_t size, void* phys_addr)
{
    if (size < MIN_BLOCK_SIZE || size > (1 << NO_CLASSES)) {
        return NULL;
    }

    size_t bag_idx = 0;

    if ((size & (size - 1)) != 0) {
        bag_idx = __builtin_clz(size) + 1;
    } else {
        bag_idx = __builtin_ffs(size) - 1;
    }

    void* ret = fetch_object(bag_idx);

    return ret;
}

void kfree(void* ptr)
{
    ASSERT_MSG((vaddr_t)ptr < pool_start && (vaddr_t)ptr >= pool_end, "Tried to free invalid pointer");

    page_hdr_t* containing_page_hdr = (page_hdr_t*)((vaddr_t)ptr & PAGE_MASK);

    if (containing_page_hdr->freelist_head_offset == BAG_FREELIST_SENTINEL) {
        size_t bag_idx = __builtin_ffs(containing_page_hdr->object_size) - 1;
        bag_t* bag     = &bags[bag_idx];

        page_hdr_t* current_hdr = bag->full;

        if (current_hdr != containing_page_hdr) {

            while (current_hdr->next != NULL && current_hdr->next != containing_page_hdr) {
                current_hdr = current_hdr->next;
            }
            ASSERT_MSG(current_hdr->next != NULL, "Page metadata not found");

            current_hdr->next = current_hdr->next->next;

            containing_page_hdr->next = bag->free;
            bag->free                 = containing_page_hdr;

        } else {
            bag->full         = current_hdr->next;
            current_hdr->next = bag->free;
            bag->free         = current_hdr;
        }
    }

    freelist_enqueue(containing_page_hdr, ptr);
}
