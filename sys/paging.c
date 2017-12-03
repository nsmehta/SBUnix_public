#include<sys/paging.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/mm.h>

Page *free_list_head;
Page *entire_list;
Page *el_end; //entire list end

uint64_t get_offset(uint64_t addr, uint64_t shift){
  return (addr >> shift) & 0x1FF;
}

void map_mem_to_pages(uint64_t end, uint64_t physfree){
  //uint64_t start = 0x0;
  uint64_t i;
  uint64_t num_pages = 0;
  uint64_t size_list = 0;
  uint64_t n_pg_list = 0;
  uint64_t start_addr_of_fl = 0;
  Page *temp;
  Page *last = NULL;
  entire_list = (Page *) physfree;
  temp = entire_list;
  for(i = 0; i < end; i += PAGESIZE){
    temp->b_addr = i;
    temp->nxtPage = temp + sizeof(Page);
    last = temp;
    temp = temp->nxtPage;
    num_pages++;
  }
  last->nxtPage = NULL;
  temp = entire_list;
  size_list = num_pages * sizeof(Page);
  if(size_list%PAGESIZE != 0){
    n_pg_list = size_list/PAGESIZE + 1; //Rounding up
  }else{
    n_pg_list = size_list/PAGESIZE;
  }
  kprintf("size of page list is : %p and number of pages used to store it%p\n", size_list, n_pg_list);
  start_addr_of_fl = physfree + n_pg_list * PAGESIZE;
  kprintf("The start of free pages is %p\n", start_addr_of_fl);
  create_free_list(start_addr_of_fl);
}

void create_free_list(uint64_t start_addr_of_fl){
  Page *temp;
  temp = entire_list;
  int sfl_flag = 0;
  while(temp){
    if(sfl_flag == 0){
      temp->status = 1;
      if(temp->nxtPage->b_addr == start_addr_of_fl){
        sfl_flag = 1;
        free_list_head = temp->nxtPage;
      }
      temp = temp->nxtPage;
    }
    else{
      temp->status = 0;
      temp = temp->nxtPage;
    }
  }
  kprintf("Free_list_head : %p\n", free_list_head->b_addr);
}

uint64_t get_next_free_page(){
  Page *temp = free_list_head;
  if(free_list_head == NULL){
    return -1;
  }else{
    free_list_head->status = 1;
    free_list_head = free_list_head->nxtPage;
    return temp->b_addr;
  }
  return -1;
}

void map_kernal(uint64_t start, uint64_t end, uint64_t paddr, pml4* pml4_t){
  uint64_t num_pages = 0;
  uint64_t i;
  uint64_t k_size = end - start;
  if(k_size % PAGESIZE == 0){
    num_pages = k_size / PAGESIZE;
  }else{
    num_pages = k_size / PAGESIZE + 1;
  }
  for(i = 0; i < num_pages; i++){
    pml4_entry(start + (i * PAGESIZE), paddr + (i * PAGESIZE), pml4_t);
  }
}

void map_video_mem(uint64_t vaddr, uint64_t paddr, pml4* pml4_t){
  pml4_entry(vaddr, paddr, pml4_t);
}

void pml4_entry(uint64_t vaddr, uint64_t paddr, pml4* pml4_t){
  uint64_t pml4off = get_offset(vaddr, PML4SHIFT);
  pdp *pdpt;
  if(*(pml4_t + pml4off) == 0){
    pdpt = (pdp *)get_next_free_page();
    if(*pdpt != -1){
      *(pml4_t + pml4off) = (uint64_t)pdpt | 0x3;
      memset((void *) pdpt, 0, PAGESIZE);
      pdpt_entry(vaddr, paddr, pdpt);
    }else{
      kprintf("Ran out of memory\n");
    }
  }else{
    pdpt = (pdp *)(*(pml4_t + pml4off) & PHYMASK);
    pdpt_entry(vaddr, paddr, pdpt);
  }
  //kprintf("Going to pdp: entry at offset is\n", *(pml4_t + pml4off));
}

void pdpt_entry(uint64_t vaddr, uint64_t paddr, pdp* pdpt){
  uint64_t pdpoff = get_offset(vaddr, PDPSHIFT);
  pd *pd_t;
  if(*(pdpt + pdpoff) == 0){
    pd_t = (pdp *)get_next_free_page();
    if(*pd_t != -1){
      *(pdpt + pdpoff) = (uint64_t)pd_t | 0x3;
      memset((void *) pd_t, 0, PAGESIZE);
      pd_entry(vaddr, paddr, pd_t);
    }else{
      kprintf("Ran out of memory\n");
    }
  }else{
    pd_t = (pd *)(*(pdpt + pdpoff) & PHYMASK);
    pd_entry(vaddr, paddr, pd_t);
  }
}

void pd_entry(uint64_t vaddr, uint64_t paddr, pd* pd_t){
  uint64_t pdoff = get_offset(vaddr, PDSHIFT);
  pt *pt_t = NULL;
  if(*(pd_t + pdoff) == 0){
    pt_t = (pdp *)get_next_free_page();
    if(*pd_t != -1){
      *(pd_t + pdoff) = (uint64_t)pt_t | 0x3;
      memset((void *) pt_t, 0, PAGESIZE);
      pt_entry(vaddr, paddr, pt_t);
    }else{
      kprintf("Ran out of memory\n");
    }
  }else{
    pt_t = (pt *)(*(pd_t + pdoff) & PHYMASK);
    pt_entry(vaddr, paddr, pt_t);
  }
}

void pt_entry(uint64_t vaddr, uint64_t paddr, pt* pt_t){
  uint64_t ptoff = get_offset(vaddr, PTSHIFT);
  *(pt_t + ptoff) = paddr | 0x3;
}

void print_va_to_pa(uint64_t vaddr,pml4* pml4_t){
  pdp *pdpt;
  pd *pd_t;
  pt *pt_t;
  uint64_t paddr, pga;
  uint64_t pml4off, pdpoff, pdoff, ptoff;
  pml4off = get_offset(vaddr, PML4SHIFT);
  pdpoff = get_offset(vaddr, PDPSHIFT);
  pdoff = get_offset(vaddr, PDSHIFT);
  ptoff = get_offset(vaddr, PTSHIFT);
  pdpt = (pdp *)(*(pml4_t + pml4off) & PHYMASK);
  pd_t = (pd *)(*(pdpt + pdpoff) & PHYMASK);
  pt_t = (pt *)(*(pd_t + pdoff) & PHYMASK);
  pga = (uint64_t)(*(pt_t + ptoff) & PHYMASK);
  paddr = pga + (vaddr & PAMASK);
  kprintf("Va :%p and pa :%p\n", vaddr, paddr);
}

void print_next_free(){
  //uint64_t addr = free_list_head;
  kprintf("next free page base is : n");
}

uint64_t kmalloc(uint64_t size){
  uint64_t flh_va = KERNBASE + (uint64_t)free_list_head;
  Page *temp = (Page *)flh_va;
  uint64_t ret_addr = KERNBASE + temp->b_addr;

  uint64_t num_pages;
  if(size % PAGESIZE == 0){
    num_pages = size/PAGESIZE;
  }else{
    num_pages = size/PAGESIZE + 1;
  }
  while(num_pages--){
    temp->status = 1;
    temp = (Page *)(KERNBASE + (uint64_t)temp->nxtPage);
  }
  free_list_head = (Page*)((uint64_t)temp - KERNBASE);
  
  //kprintf("New flh is : %p\n", free_list_head);
  return ret_addr;
}
