#include <arch/multiboot.h>
#include <arch/paging.h>
#include <kern/kprint.h>
#include <types.h>

#define FREE_PAGEFRAME_STACK_SIZE 4096

static unsigned int free_frames_stack[FREE_PAGEFRAME_STACK_SIZE] = { 0 };
static int          free_frames_stack_top                        = -1;

static unsigned int frames_in_use    = 0;
static unsigned int frames_available = 0;

static unsigned int mmap_length   = 0;
static unsigned int mmap_addr     = 0;
static unsigned int mmap_end_addr = 0;

static unsigned int next_free_frame_num = 1;

void pageframe_alloc_init(struct multiboot_info* multiboot_info_ptr)
{
    mmap_length   = multiboot_info_ptr->mmap_length;
    mmap_addr     = multiboot_info_ptr->mmap_addr;
    mmap_end_addr = mmap_addr + mmap_length;

    frames_in_use    = 0;
    frames_available = multiboot_info_ptr->mem_upper / 4;

    kprintf("Detected %d MB of free memory\n", multiboot_info_ptr->mem_upper / 1024);
    kprintf("Detected %d free frames\n", frames_available);
}

unsigned int mmap_read_addr(unsigned int addr)
{
    unsigned int            curr_addr = mmap_addr;
    multiboot_memory_map_t* entry;

    unsigned int current_frame_num = 0;

    while (curr_addr < mmap_end_addr) {
        entry = (multiboot_memory_map_t*)curr_addr;

        unsigned int region_addr_start = entry->addr;
        unsigned int region_addr_end   = entry->addr + entry->len;
        unsigned int region_no_frames  = entry->len / PAGE_SIZE;

        if (addr >= region_addr_start
            && addr <= region_addr_end) {

            unsigned int ret_num = current_frame_num + ((addr - region_addr_start) / PAGE_SIZE);
            return ret_num;
        }

        current_frame_num += region_no_frames;
        curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
    }

    return 0;
}

unsigned int mmap_scan_free(unsigned int requested_frame_num, unsigned int* res_frame_num)
{
    unsigned int            curr_addr = mmap_addr;
    multiboot_memory_map_t* entry;

    unsigned int current_frame_num = 0;

    while (curr_addr < mmap_end_addr) {
        entry = (multiboot_memory_map_t*)curr_addr;

        unsigned int region_addr_start = entry->addr;
        unsigned int region_addr_end   = entry->addr + entry->len;
        unsigned int region_no_frames  = entry->len / PAGE_SIZE;

        if (requested_frame_num >= current_frame_num
            && requested_frame_num <= (current_frame_num + region_no_frames)) {

            if (entry->type != MULTIBOOT_MEMORY_AVAILABLE || (mmap_addr >= region_addr_start && mmap_addr <= region_addr_end)) {
                current_frame_num += region_no_frames;
                requested_frame_num += region_no_frames;
                curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
                continue;
            }

            unsigned int ret_addr = region_addr_start + ((requested_frame_num - current_frame_num) * PAGE_SIZE);
            *res_frame_num        = requested_frame_num;
            return ret_addr;
        }

        current_frame_num += region_no_frames;
        curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
    }

    return 0;
}

unsigned int alloc_frame(void)
{
    unsigned int frame_num;
    unsigned int frame_addr;

    if (free_frames_stack_top >= 0) {
        frame_addr = free_frames_stack[free_frames_stack_top];
        free_frames_stack_top--;
        return frame_addr;
    }

    frame_addr = mmap_scan_free(next_free_frame_num, &frame_num);

    // TODO: handle panic
    next_free_frame_num = frame_num + 1;

    return frame_addr;
}

void free_frame(unsigned int addr)
{
    if (free_frames_stack_top < FREE_PAGEFRAME_STACK_SIZE) {
        free_frames_stack_top++;
        free_frames_stack[free_frames_stack_top] = addr;
    }
}
