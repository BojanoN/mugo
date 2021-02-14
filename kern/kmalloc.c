#include <kern/util.h>
#include <types.h>

/*
 * A buddy allocator.
 */

#define NO_SIZE_CLASSES 10
#define MIN_BLOCK_SIZE  sizeof(word)
#define SENTINEL        WORD_MAX

static uint8_t* pool_start_vaddr = NULL;
static uint8_t* pool_end_vaddr   = NULL;

static uint8_t* pool_start_phys = NULL;

static word size_freelists[NO_SIZE_CLASSES] = { SENTINEL };

typedef word kmalloc_header_t;

static inline void split_block(uint8_t* block, int class_index)
{
    size_t new_block_size  = (1 << (class_index - 1)) * MIN_BLOCK_SIZE;
    word   new_block       = (word)(block + new_block_size);
    word   next_free_block = *((word*)size_freelists[class_index]);

    size_freelists[class_index] = next_free_block;

    *((word*)(next_free_block)) = size_freelists[class_index - 1];
    *((word*)(block))           = new_block;
}

static inline void merge_blocks(uint8_t* block1, int class_index1, uint8_t* block2, int class_index2)
{
}

void kmalloc_init(uint8_t* pool_vaddr, uint8_t* pool_phys, size_t size)
{
    pool_start_vaddr = pool_vaddr;
    pool_end_vaddr   = pool_vaddr + size;

    pool_start_phys = pool_phys;

    size_freelists[NO_SIZE_CLASSES - 1] = (word)pool_vaddr;
}

void* kmalloc(size_t size, void* phys_addr)
{
    int size_class_index = 0;
    size += sizeof(kmalloc_header_t);

    // Round to power of 2 if needed
    if (__builtin_popcountl(size) > 1) {
        size_class_index = __builtin_clz(size) + 1;
    } else {
        size_class_index = __builtin_ffs(size) - 1;
    }

    word free_block = 0;

    if (size_freelists[size_class_index] != SENTINEL) {
        free_block                       = size_freelists[size_class_index];
        size_freelists[size_class_index] = *((word*)free_block);
    } else {
        int i                           = 0;
        int first_available_class_index = 0;

        for (i = size_class_index; i < NO_SIZE_CLASSES; i++) {
            if (size_freelists[i] != SENTINEL) {
                first_available_class_index = i;
                break;
            }
        }

        if (i == NO_SIZE_CLASSES) {
            panic("Kernel allocator: OUT OF MEMORY");
            return NULL;
        }

        // split block
        while (first_available_class_index > size_class_index) {
            split_block((uint8_t*)size_freelists[first_available_class_index], first_available_class_index);
            first_available_class_index--;
        }

        free_block                       = size_freelists[size_class_index];
        size_freelists[size_class_index] = *((word*)free_block);
    }

    if (phys_addr != NULL) {
        *((word*)phys_addr) = (word)(pool_start_phys + ((uint8_t*)free_block - pool_start_vaddr));
    }

    *((word*)free_block) = size_class_index;
    return (void*)(free_block + sizeof(kmalloc_header_t));
}

void kfree(void* ptr)
{
    uint8_t* target_ptr = (uint8_t*)(ptr - sizeof(kmalloc_header_t));
    if (!ptr || (target_ptr > pool_end_vaddr || target_ptr < pool_start_vaddr)) {
        panic("Invalid free detected");
        return;
    }

    int size_class = *(int*)(target_ptr);

    *((word*)target_ptr)       = size_freelists[size_class];
    size_freelists[size_class] = (word)(target_ptr);
}
