#ifndef __PAGING_H
#define __PAGING_H
#include<sys/defs.h>

#define ALLOCATED 1
#define FREE 0
#define PAGESIZE 4096UL
#define PML4SHIFT 39
#define PDPSHIFT 30
#define PDSHIFT 21
#define PTSHIFT 12
#define PAMASK 0xFFFUL
#define PHYMASK 0xfffffffffffff000UL

#define PAGE_PRESENT  1 << 0
#define PAGE_RW       1 << 1
#define PAGE_US       1 << 2
#define PAGE_ACCESSED 1 << 5
#define PAGE_DIRTY    1 << 6

#define PAGE_KERNEL (PAGE_PRESENT | PAGE_RW | PAGE_US)
#define PAGE_USER (PAGE_PRESENT | PAGE_RW | PAGE_US)



// #define PML4_PT_WALK  0xfffffffffffff000UL
// #define PDPE_PT_WALK  0xffffffffffe00000UL
// #define PGD_PT_WALK   0xffffffffc0000000UL
// #define PTE_PT_WALK   0xffffff8000000000UL

#define PML4_PT_WALK  0xffffff7fbfdfe000UL
#define PDPE_PT_WALK  0xffffff7fbfc00000UL
#define PGD_PT_WALK   0xffffff7f80000000UL
#define PTE_PT_WALK   0xffffff0000000000UL

// #define PML4_OFFSET   0x0000ff8000000000UL
// #define PDPE_OFFSET   0x0000ffffc0000000UL
// #define PGD_OFFSET    0x0000ffffffe00000UL
// #define PTE_OFFSET    0x0000fffffffff000UL

#define PML4_OFFSET   0x0000ff8000000000UL
#define PDPE_OFFSET   0x0000ffffc0000000UL
#define PGD_OFFSET    0x0000ffffffe00000UL
#define PTE_OFFSET    0x0000fffffffff000UL


uint64_t kernel_cr3;
uint64_t top_virtual_address;
uint64_t *kernel_pml4_t;
uint64_t paging_user_stack_top;

typedef struct Page{
  struct Page *next;
  uint64_t status; // 0 - free and 1 - allocated
  uint64_t p_addr;
}Page;

void create_free_list();
void map_mem_to_pages(uint64_t, uint64_t);
void create_free_list(uint64_t);
uint64_t get_next_free_page();
uint64_t get_offset(uint64_t , uint64_t );
void map_kernel(uint64_t, uint64_t, uint64_t, pml4* pml4_t);
void pml4_entry(uint64_t, uint64_t, pml4*);
void pdpt_entry(uint64_t, uint64_t , pdp*);
void pd_entry(uint64_t, uint64_t , pd*);
void pt_entry(uint64_t, uint64_t , pt*);
void print_va_to_pa(uint64_t ,pml4*);
void map_video_mem(uint64_t , uint64_t , pml4*);
void print_next_free();
uint64_t kmalloc_old(uint64_t);
void set_cr3(pml4*);
uint64_t get_cr3();
uint64_t get_cr2();
uint64_t align_up(uint64_t);
uint64_t align_down(uint64_t);
void page_fault_handler();


uint64_t *create_kernel_pml4();
void map_all_kernel_pages(uint64_t v_addr_start, uint64_t v_addr_end, uint64_t p_addr_start, pml4 *kernel_pml4);
void map_kernel_pages(uint64_t v_addr_start, uint64_t v_addr_end, uint64_t p_addr, pml4 *kernel_pml4);
void map_kernel_page(uint64_t v_addr, uint64_t p_addr, pml4 *kernel_pml4);
void map_pdpt_entry(uint64_t v_addr, uint64_t p_addr, pml4 *kernel_pml4);
int is_valid_page(uint64_t page_address);
uint64_t get_free_list_head();
uint64_t get_free_list_page_length();

uint64_t to_virtual(uint64_t phy_add);
uint64_t to_physical(uint64_t vir_add);

void set_top_virtual_address(uint64_t v_addr);
uint64_t kmalloc(size_t size);
uint64_t kmalloc_user(size_t size);
int page_walk(uint64_t p_addr, uint64_t v_addr);
uint64_t get_next_free_page_kmalloc();
void set_new_free_list_head();
uint64_t page_align(uint64_t addr);
void print_free_list();
void print_free_list_kmalloc();

void initialize_free_list();
void append_free_list(uint64_t start, uint64_t end, uint64_t physfree);
void set_free_list_head();
uint64_t round_up(uint64_t addr);
uint64_t create_new_kernel_pml4(uint64_t p_addr, uint64_t* pml4_t);
uint64_t create_new_user_pml4(uint64_t p_addr, uint64_t* pml4_t);
int page_walk_user(uint64_t p_addr, uint64_t v_addr);
uint64_t kmalloc_top_virtual_address_kernel(size_t size, uint64_t top_virtual_address);
uint64_t kmalloc_top_virtual_address_user(size_t size, uint64_t top_virtual_address);

#endif
