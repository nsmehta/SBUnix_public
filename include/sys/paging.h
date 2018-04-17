#ifndef __PAGING_H
#define __PAGING_H
#include<sys/defs.h>

#define ALLOCATED 1
#define FREE 0
#define PAGESIZE 4096
#define PML4SHIFT 39
#define PDPSHIFT 30
#define PDSHIFT 21
#define PTSHIFT 12
#define PAMASK 0xFFF
#define PHYMASK 0xfffffffffffff000

#define PAGE_PRESENT  1 << 0
#define PAGE_RW       1 << 1
#define PAGE_US       1 << 2
#define PAGE_ACCESSED 1 << 5
#define PAGE_DIRTY    1 << 6

#define PAGE_KERNEL (PAGE_PRESENT | PAGE_RW)
#define PAGE_USER (PAGE_PRESENT | PAGE_RW | PAGE_US)

#define PML4_OFFSET 0xfffffffffffff000
#define PDPE_OFFSET 0xffffffffffe00000
#define PGD_OFFSET  0xffffffffc0000000
#define PTE_OFFSET  0xffffff8000000000

uint64_t kernel_cr3;
uint64_t *top_virtual_address;

typedef struct Page{
  struct Page *next;
  int status; // 0 - free and 1 - allocated
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
uint64_t kmalloc(uint64_t);
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

#endif
