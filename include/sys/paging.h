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

typedef struct Page{
  struct Page *nxtPage;
  int status; //0 - free and 1 - allocated
  uint64_t b_addr;
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
#endif
