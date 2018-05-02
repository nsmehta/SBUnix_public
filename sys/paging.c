#include<sys/paging.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/mm.h>

Page *free_list_head;
Page *free_list_head_kmalloc;
Page *free_list;
uint64_t free_list_page_length;
uint64_t free_list_total_pages;

// return page aligned virtual address
uint64_t to_virtual(uint64_t phy_add) {
  if (phy_add < (uint64_t)KERNBASE) {
    return ((phy_add / 0x1000) * 0x1000) + (uint64_t)KERNBASE;
  }
  return phy_add;
}

uint64_t page_align(uint64_t addr) {
  return ((addr / 0x1000) * 0x1000);
}

uint64_t round_up(uint64_t addr) {
  if (addr % PAGESIZE) {
    return (((addr / 0x1000) * 0x1000) + 0x1000);
  } else {
    return ((addr / 0x1000) * 0x1000);
  }
}

// return page aligned physical address
uint64_t to_physical(uint64_t vir_add) {
  return ((vir_add / 0x1000) * 0x1000) - (uint64_t)KERNBASE;
}


void initialize_free_list() {
  free_list = NULL;
  free_list_total_pages = 0;
}

void append_free_list(uint64_t start, uint64_t end, uint64_t physfree) {
  // rounding up the addresses
  start = round_up(start);
  end = round_up(end);
  physfree = round_up(physfree);
  
  if(free_list == NULL) {
    if(start <= physfree && end >= physfree) {
      free_list = (Page *) physfree;
      Page *temp = free_list;
      Page *last = NULL;
      for(uint64_t i = physfree; i < end; i += (uint64_t)PAGESIZE) {
        memset((void *) i, 0, (uint64_t)PAGESIZE/(uint64_t)8);
        memset((void *) ((uint64_t)KERNBASE + i), 0, (uint64_t)PAGESIZE/(uint64_t)8);
        temp->p_addr = i;
        temp->next = (Page *)((uint64_t)temp + sizeof(Page));
        last = temp;
        temp = temp->next;
        free_list_total_pages++;
      }
//      last->next = NULL;
      temp = last;
      temp->next = NULL;
      kprintf("last = %p\n", last->p_addr);
    }
  } else {
    if(start >= physfree) {
      Page *temp = free_list;
      Page *last = NULL;
      while(temp->next != NULL) {
        temp = temp->next;
      }
      last = temp;
      for(uint64_t i = start; i <= end; i += (uint64_t)PAGESIZE) {
        temp->next = (Page *)((uint64_t)temp + sizeof(Page));
        temp = temp->next;
        memset((void *) i, 0, (uint64_t)PAGESIZE/(uint64_t)8);
        memset((void *) ((uint64_t)KERNBASE + i), 0, (uint64_t)PAGESIZE/(uint64_t)8);
        temp->p_addr = i;
        last = temp;
        free_list_total_pages++;
      }
      temp = last;
      temp->next = NULL;
      kprintf("last = %p\n", last->p_addr);
    }
  }
}

void set_free_list_head() {
  uint64_t free_list_size = free_list_total_pages * sizeof(Page);
  uint64_t free_list_pages_reqd = 0;
  
  kprintf("free_list_page_length = %p\n", free_list_page_length);
  
  if(free_list_size % (uint64_t)PAGESIZE) {
    free_list_pages_reqd = free_list_size / (uint64_t)PAGESIZE + 1;
  } else {
    free_list_pages_reqd = free_list_size / (uint64_t)PAGESIZE;
  }
  
  Page *temp = free_list;
  for(uint64_t i = 0; i < free_list_pages_reqd; i++) {
    temp = temp->next;
  }
  free_list_head = temp;
  free_list_page_length = free_list_pages_reqd;
  free_list_total_pages -= free_list_pages_reqd;
  kprintf("free_list_total_pages = %p\n", free_list_total_pages);
  
  kprintf("free_list_head = %p, sizeof(Page) = %p\n", free_list_head, sizeof(Page));
  
}

// creates the free list: list of free and allocated physical pages
void map_mem_to_pages(uint64_t end, uint64_t physfree) {
  uint64_t num_pages = 0;
  uint64_t free_list_size = 0;
  uint64_t free_list_start_address = 0;
  free_list_page_length = 0;
  Page *temp;
  Page *last = NULL;
  free_list = (Page *) physfree;
  temp = free_list;
  // created a linked list of all the pages
  // also calculate the number of pages
  for(uint64_t i = 0; i < end; i += (uint64_t)PAGESIZE) {
    temp->p_addr = i;
    temp->next = (Page *)((uint64_t)temp + sizeof(Page));
    last = temp;
    temp = temp->next;
    num_pages++;
  }
  
  last->next = NULL;
  temp = free_list;
  
  // size of the page descriptor list
  free_list_size = num_pages * sizeof(Page);
  
  kprintf("num pages = %p, sizeof Page = %p\n", num_pages, sizeof(Page));
  
  // calculate the number of pages required to store the linked list of all pages
  if(free_list_size % (uint64_t)PAGESIZE != 0) {
    free_list_page_length = free_list_size / (uint64_t)PAGESIZE + 1; //Rounding up
  }
  else {
    free_list_page_length = free_list_size / (uint64_t)PAGESIZE;
  }

  
  kprintf("size of page list is : %p and number of pages used to store it: %p\n", free_list_size, free_list_page_length);
  free_list_start_address = physfree + free_list_page_length * (uint64_t)PAGESIZE;
  kprintf("The start of free pages is %p\n", free_list_start_address);
  
  // create linked list of free and allocated pages
  create_free_list(free_list_start_address);
}

// maps all pages above the free list (stored above physfree) as free
// and from physfree to end of free list as allocated
void create_free_list(uint64_t free_list_start_address) {
  Page *temp;
  temp = free_list;

  int free_list_status = ALLOCATED;
  while(temp) {
    if (!temp->next) {
      kprintf("last = %p, p_addr = %p\n", temp, temp->p_addr);
    }
    if(free_list_status == ALLOCATED) {
      temp->status = ALLOCATED;
//      kprintf("%d %p | ", temp->status, temp->p_addr);
      if(temp->next->p_addr == free_list_start_address) {
        free_list_status = FREE;
        free_list_head = temp->next;
      }
      temp = temp->next;
    }
    else {
//      kprintf("add: %p |", temp);
      temp->status = FREE;
//      kprintf("%d %p | ", temp->status, temp->p_addr);
      temp = temp->next;
    }
    
  }
  kprintf("Free_list_head->p_addr : %p\n", free_list_head->p_addr);
}

uint64_t get_free_list_head() {
  return free_list_head->p_addr;
}

uint64_t get_free_list_page_length() {
  return free_list_page_length;
}

// returns PHYSICAL address of the next free page from the free list
uint64_t get_next_free_page() {
  Page *temp = free_list_head;
  if(free_list_head == NULL) {
    return -1;
  }
  else {
    free_list_head->status = ALLOCATED;
    free_list_head = free_list_head->next;
    return temp->p_addr;
  }
  return -1;
}


void set_new_free_list_head() {
  
  free_list_head_kmalloc = (Page *)((uint64_t)free_list_head + KERNBASE);
  
}

void print_free_list() {
  Page *temp = free_list;
  int i = 0;
  int free_c = 0;
  int all_c = 0;
  while(temp != NULL) {
    if (temp->status == FREE) {
      free_c++;
    } else {
      all_c++;
    }
    
//    if (i > 500 && i < 600) {
//      kprintf("%d: %d %p| ", i, temp->status, (uint64_t)temp);  
//    }
    
    if ((Page *)((uint64_t)temp->next) != NULL && (((Page *)((uint64_t)temp->next))->p_addr < temp->p_addr)) {
      kprintf("lower!!\n");
    }
    
//    free_list_head_kmalloc->status = ALLOCATED;
    
    temp = (Page *)((uint64_t)temp->next);
    i++;
  }
  kprintf("i = %d, free = %d, all = %d\n", i, free_c, all_c);
  
}

void print_free_list_kmalloc() {
  Page *temp = free_list_head_kmalloc;
  int i = 0;
  while(temp != NULL) {
    
    kprintf("%p %p|\n", temp, temp->p_addr);
    
//    free_list_head_kmalloc->status = ALLOCATED;
    Page *next = (temp->next);
    if (next == NULL) {
      kprintf("NULL! = %p\n", (uint64_t)next);
      break;
    }
    next = (Page *)(KERNBASE + (uint64_t)temp->next);
    if (next != NULL && next < temp) {
      kprintf("lower! %p\n", next);
      break;
    }
    temp = (Page *)(KERNBASE + (uint64_t)temp->next);
    i++;
  }
  kprintf("%p, %p\n", temp, temp->p_addr);
  kprintf("i = %d\n", i);
  
}


// returns PHYSICAL address of the next free page from the free list
uint64_t get_next_free_page_kmalloc() {
  Page *temp;
  temp = free_list_head_kmalloc;
  if(free_list_head_kmalloc == NULL) {
    return 0;
  }
  else {
    free_list_head_kmalloc->status = ALLOCATED;
    free_list_head_kmalloc = (Page *)(KERNBASE + (uint64_t)free_list_head_kmalloc->next);
    return temp->p_addr;
  }
}




void set_cr3(pml4* pml4_t) {
//    kprintf("cr3 = %p\n", pml4_t);
  __asm__ volatile("mov %0, %%cr3"::"b"(pml4_t));
}

uint64_t get_cr2() {
  uint64_t current_cr2;
  __asm__ volatile("mov %%cr2, %0":"=r"(current_cr2));
  return current_cr2;
}


uint64_t get_cr3() {
  uint64_t current_cr3;
  __asm__ volatile("mov %%cr3, %0":"=r"(current_cr3));
  return current_cr3;
}

uint64_t *create_kernel_pml4() {
  uint64_t pml4_t = get_next_free_page();
//  kprintf("kernel cr3 = %p\n", kernel_cr3);

  // CR3 register contains the physical address of the base address of the page directory table
  kernel_cr3 = pml4_t;
//  kprintf("kernel cr3 = %p\n", kernel_cr3);
  
  // kernel pml4 will have a virtual address
  uint64_t *kernel_pml4 = (uint64_t *)(pml4_t + KERNBASE);
  
  
  kernel_pml4[510] = kernel_cr3 | PAGE_KERNEL;
  kernel_pml4_t = kernel_pml4;
//  kernel_pml4[510] = kernel_cr3 | PAGE_KERNEL;
  
//  kprintf("kernel pml4 = %p, kernel_pml4[511] = %p\n", kernel_pml4, kernel_pml4[511]);
  return kernel_pml4;
}

// mapping all pages from physbase to physfree + free list
void map_all_kernel_pages(uint64_t v_addr_start, uint64_t v_addr_end, uint64_t p_addr_start, pml4 *kernel_pml4) {
  //offsets:
  
  kprintf("v_addr = %p, p_addr = %p\n", v_addr_start, p_addr_start);
  v_addr_start = page_align(v_addr_start);
  p_addr_start = page_align(p_addr_start);
  
  for (; v_addr_start <= v_addr_end; v_addr_start += (uint64_t)PAGESIZE, p_addr_start += (uint64_t)PAGESIZE) {
    

    uint64_t pml4_offset = (v_addr_start >> 39) & 0x1ff;
    uint64_t pdpe_offset = (v_addr_start >> 30) & 0x1ff;
    uint64_t pgd_offset = (v_addr_start >> 21) & 0x1ff;
    uint64_t pte_offset = (v_addr_start >> 12) & 0x1ff;

    uint64_t *pdpe_base_add = (uint64_t *) kernel_pml4[pml4_offset];

    
    // if pdpe is present:
    if(is_valid_page((uint64_t)pdpe_base_add)) {

      uint64_t *pgd_base_add = (uint64_t *) ((uint64_t *)to_virtual((uint64_t)pdpe_base_add))[pdpe_offset];

      // if pgd is present:
      if(is_valid_page((uint64_t)pgd_base_add)) {

        uint64_t *pte_base_add = (uint64_t *) ((uint64_t *)to_virtual((uint64_t)pgd_base_add))[pgd_offset];

        // if pte is present:
        if(is_valid_page((uint64_t)pte_base_add)) {
          // the pt entry that points to the page

          uint64_t *pte_vir_add = (uint64_t *)(to_virtual((uint64_t) pte_base_add));

          pte_vir_add[pte_offset] = p_addr_start  | PAGE_KERNEL;

        } else {
          
          uint64_t pte_base_add = get_next_free_page();
          ((uint64_t *)to_virtual((uint64_t)pgd_base_add))[pgd_offset] = pte_base_add | PAGE_KERNEL;
          uint64_t *pte_vir_add = (uint64_t *)(to_virtual(pte_base_add));
          pte_vir_add[pte_offset] = p_addr_start  | PAGE_KERNEL;

        }
      } else {
        
        uint64_t pgd_base_add = get_next_free_page();

        ((uint64_t *)to_virtual((uint64_t)pdpe_base_add))[pdpe_offset] = pgd_base_add | PAGE_KERNEL;

        uint64_t *pgd_vir_add = (uint64_t *)(to_virtual(pgd_base_add));
        
        uint64_t pte_base_add = get_next_free_page();
        pgd_vir_add[pgd_offset] = pte_base_add | PAGE_KERNEL;
        
        uint64_t *pte_vir_add = (uint64_t *)(to_virtual(pte_base_add));
        pte_vir_add[pte_offset] = p_addr_start | PAGE_KERNEL;

      }
    } else {
      
      uint64_t pdpe_base_add = get_next_free_page();
      kernel_pml4[pml4_offset] = pdpe_base_add | PAGE_KERNEL;
      uint64_t *pdpe_vir_add = (uint64_t *)(to_virtual(pdpe_base_add));
      
      uint64_t pgd_base_add = get_next_free_page();
      pdpe_vir_add[pdpe_offset] = pgd_base_add | PAGE_KERNEL;
      uint64_t *pgd_vir_add = (uint64_t *)(to_virtual(pgd_base_add));

      uint64_t pte_base_add = get_next_free_page();
      pgd_vir_add[pgd_offset] = pte_base_add | PAGE_KERNEL;
      uint64_t *pte_vir_add = (uint64_t *)(to_virtual(pte_base_add));
      pte_vir_add[pte_offset] = p_addr_start | PAGE_KERNEL;

    }
  }
}


void set_top_virtual_address(uint64_t v_addr) {
  top_virtual_address = v_addr;
  kprintf("top virtual address = %p\n", top_virtual_address);
  paging_user_stack_top = 0xfffffe0000000000 + 0x1000 - 0x8;
}

uint64_t kmalloc(size_t size) {
  
  uint64_t no_of_pages = 0;
  
  if (size % (uint64_t)PAGESIZE != 0) {
    no_of_pages = (size / (uint64_t)PAGESIZE) + 1;
  } else {
    no_of_pages = (size / (uint64_t)PAGESIZE);
  }
  
  uint64_t top = top_virtual_address;
  uint64_t v_addr = top;
  

  for(uint64_t i = 0; i < no_of_pages; i++) {

    uint64_t p_addr = get_next_free_page_kmalloc();
    if (p_addr == 0) {
      kprintf("no free pages left! %p\n", i);
      return 0;
    }
    kprintf("next free = %p\n", p_addr);

    int result = page_walk(p_addr, v_addr);

    if (result == 0) {
      kprintf("kmalloc failed! i = %p\n", i);
      return 0;
    }
    v_addr += (uint64_t)PAGESIZE;
    top_virtual_address += (uint64_t)PAGESIZE;
  }
  
  return top;
  
}

// kmalloc for user processes
uint64_t kmalloc_user(size_t size) {
  
  uint64_t no_of_pages = 0;
  
  if (size % (uint64_t)PAGESIZE != 0) {
    no_of_pages = (size / (uint64_t)PAGESIZE) + 1;
  } else {
    no_of_pages = (size / (uint64_t)PAGESIZE);
  }
  no_of_pages++;
  
  uint64_t top = top_virtual_address;
  uint64_t v_addr = top;
  

  for(uint64_t i = 0; i < no_of_pages; i++) {

    uint64_t p_addr = get_next_free_page_kmalloc();
    if (p_addr == 0) {
      kprintf("no free pages left! %p\n", i);
      return 0;
    }
    // kprintf("next user free = %p, vaddr = %p\n", p_addr, v_addr);

    int result = page_walk_user(p_addr, v_addr);

    if (result == 0) {
      kprintf("kmalloc failed! i = %p\n", i);
      return 0;
    }
    v_addr += (uint64_t)PAGESIZE;
    top_virtual_address += (uint64_t)PAGESIZE;
  }
  
  return top;
  
}


// Kmalloc for a different top virtual address:
uint64_t kmalloc_top_virtual_address_kernel(size_t size, uint64_t new_top_virtual_address) {
  
  uint64_t no_of_pages = 0;
  
  if (size % (uint64_t)PAGESIZE != 0) {
    no_of_pages = (size / (uint64_t)PAGESIZE) + 1;
  } else {
    no_of_pages = (size / (uint64_t)PAGESIZE);
  }
  
  uint64_t top = new_top_virtual_address;
  uint64_t v_addr = top;
  

  for(uint64_t i = 0; i < no_of_pages; i++) {

    uint64_t p_addr = get_next_free_page_kmalloc();
    if (p_addr == 0) {
      kprintf("no free pages left! %p\n", i);
      return 0;
    }
    kprintf("next free = %p\n", p_addr);

    int result = page_walk(p_addr, v_addr);

    if (result == 0) {
      kprintf("kmalloc failed! i = %p\n", i);
      return 0;
    }
    v_addr += (uint64_t)PAGESIZE;
    new_top_virtual_address += (uint64_t)PAGESIZE;
  }
  
  return top;  
  
}


// Kmalloc for a different top virtual address:
uint64_t kmalloc_top_virtual_address_user(size_t size, uint64_t user_top_virtual_address) {
  
  uint64_t no_of_pages = 0;
  
  if (size % (uint64_t)PAGESIZE != 0) {
    no_of_pages = (size / (uint64_t)PAGESIZE) + 1;
  } else {
    no_of_pages = (size / (uint64_t)PAGESIZE);
  }
  
  uint64_t top = user_top_virtual_address;
  uint64_t v_addr = top;

  kprintf("top virtual address = %p, size = %p, pages = %p\n", top, size, no_of_pages);
  

  for(uint64_t i = 0; i < no_of_pages; i++) {

    uint64_t p_addr = get_next_free_page_kmalloc();
    kprintf("p_addr = %p\n", p_addr);
    if (p_addr == 0) {
      kprintf("no free pages left! %p\n", i);
      return 0;
    }
    // kprintf("next user page walk free = %p\n", p_addr);

    int result = page_walk_user(p_addr, v_addr);

    if (result == 0) {
      kprintf("kmalloc failed! i = %p\n", i);
      return 0;
    }
    v_addr += (uint64_t)PAGESIZE;
    // top_virtual_address += (uint64_t)PAGESIZE;
  }
  
  return top;  
  
}


// page walks the virtual address and maps the allocated physical address
int page_walk(uint64_t p_addr, uint64_t v_addr) {
  uint64_t pml4_part = (v_addr & (uint64_t)PML4_OFFSET) >> 36;
  uint64_t pdpe_part = (v_addr & (uint64_t)PDPE_OFFSET) >> 27;
  uint64_t pgd_part = (v_addr & (uint64_t)PGD_OFFSET) >> 18;
  uint64_t pte_part = (v_addr & (uint64_t)PTE_OFFSET) >> 9;
  
  uint64_t *pml4_rec_addr = (uint64_t *) (pml4_part | (uint64_t)PML4_PT_WALK);
  uint64_t *pdpe_rec_addr = (uint64_t *) (pdpe_part | (uint64_t)PDPE_PT_WALK);
  uint64_t *pgd_rec_addr = (uint64_t *) (pgd_part | (uint64_t)PGD_PT_WALK);
  uint64_t *pte_rec_addr = (uint64_t *) (pte_part | (uint64_t)PTE_PT_WALK);
  
  
  // kprintf("pml4_rec_addr = %p\n", pml4_rec_addr);
  // kprintf("pdpe_rec_addr = %p\n", pdpe_rec_addr);
  // kprintf("pgd_rec_addr = %p\n", pgd_rec_addr);
  // kprintf("pte_rec_addr = %p\n", pte_rec_addr);

//  kprintf("cr3 = %p\n", kernel_cr3);

//  kprintf("pml4_rec_addr = %p, *pml4_rec_addr = %p\nvalue = %p\n", (uint64_t)pml4_rec_addr, *pml4_rec_addr, (uint64_t)pml4_rec_addr);

//  kprintf("pml4 = %p, v_addr = %p, \nvalue = %p, p_addr = %p\n", pml4_rec_addr, v_addr, *pml4_rec_addr, p_addr);

  if (is_valid_page((uint64_t) *pml4_rec_addr)) {

    if (is_valid_page((uint64_t) *pdpe_rec_addr)) {
      

      if (is_valid_page((uint64_t) *pgd_rec_addr)) {
        
//        kprintf("pte = %p\n", pte_rec_addr);

        
        if(is_valid_page((uint64_t) *pte_rec_addr)) {
          
          kprintf("valid pte! = %p\n", (uint64_t) *pte_rec_addr);
          // the virtual page is already mapped! we will need to free the physical page and return 0
          return 0;
          
        } else {
//          kprintf("invalid pte! = %p\n", (uint64_t) *pte_rec_addr);

          *pte_rec_addr = p_addr | PAGE_KERNEL;
          return 1;

        }
        
      } else {
      

        uint64_t physical_page = get_next_free_page_kmalloc();

        *pgd_rec_addr = physical_page | PAGE_KERNEL;

        *pte_rec_addr = p_addr | PAGE_KERNEL;

        return 1;
        
      }
      
    } else {
    

      uint64_t physical_page = get_next_free_page_kmalloc();
      *pdpe_rec_addr = physical_page | PAGE_KERNEL;
      physical_page = get_next_free_page_kmalloc();
      *pgd_rec_addr = physical_page | PAGE_KERNEL;

      *pte_rec_addr = p_addr | PAGE_KERNEL;
      return 1;

    }
    
  } else {
    
    kprintf("1. not valid pml4\n");

    uint64_t physical_page = get_next_free_page_kmalloc();

    *pml4_rec_addr = physical_page | PAGE_KERNEL;

    physical_page = get_next_free_page_kmalloc();

    *pdpe_rec_addr = physical_page | PAGE_KERNEL;
    physical_page = get_next_free_page_kmalloc();

    *pgd_rec_addr = physical_page | PAGE_KERNEL;

    *pte_rec_addr = p_addr | PAGE_KERNEL;

    return 1;

  }

}


// page walks the virtual address and maps the allocated physical address for user processes
int page_walk_user(uint64_t p_addr, uint64_t v_addr) {
  uint64_t pml4_part = (v_addr & (uint64_t)PML4_OFFSET) >> 36;
  uint64_t pdpe_part = (v_addr & (uint64_t)PDPE_OFFSET) >> 27;
  uint64_t pgd_part = (v_addr & (uint64_t)PGD_OFFSET) >> 18;
  uint64_t pte_part = (v_addr & (uint64_t)PTE_OFFSET) >> 9;
  
  uint64_t *pml4_rec_addr = (uint64_t *) (pml4_part | (uint64_t)PML4_PT_WALK);
  uint64_t *pdpe_rec_addr = (uint64_t *) (pdpe_part | (uint64_t)PDPE_PT_WALK);
  uint64_t *pgd_rec_addr = (uint64_t *) (pgd_part | (uint64_t)PGD_PT_WALK);
  uint64_t *pte_rec_addr = (uint64_t *) (pte_part | (uint64_t)PTE_PT_WALK);
  
  // kprintf("pml4_rec_addr = %p\n", pml4_rec_addr);
  // kprintf("pdpe_rec_addr = %p\n", pdpe_rec_addr);
  // kprintf("pgd_rec_addr = %p\n", pgd_rec_addr);
  // kprintf("pte_rec_addr = %p\n", pte_rec_addr);

  

 // kprintf("cr3 = %p\n", get_cr3());

 // kprintf("pml4_rec_addr = %p, *pml4_rec_addr = %p\n", (uint64_t)pml4_rec_addr, *pml4_rec_addr);
 // kprintf("pdpe_rec_addr = %p\n", (uint64_t)pdpe_rec_addr);
 // kprintf("pgd_rec_addr = %p\n", (uint64_t)pgd_rec_addr);
 // kprintf("pte_rec_addr = %p\n", (uint64_t)pte_rec_addr);

 // kprintf("pml4 = %p, v_addr = %p, \nvalue = %p, p_addr = %p\n", pml4_rec_addr, v_addr, *pml4_rec_addr, p_addr);

  if (is_valid_page((uint64_t) *pml4_rec_addr)) {

    if (is_valid_page((uint64_t) *pdpe_rec_addr)) {
      

      if (is_valid_page((uint64_t) *pgd_rec_addr)) {
        
        
        if(is_valid_page((uint64_t) *pte_rec_addr)) {
          
          kprintf("valid pte! = %p\n", (uint64_t) *pte_rec_addr);
          // the virtual page is already mapped! we will need to free the physical page and return 0
          return 0;
          
        } else {
//          kprintf("invalid pte! = %p\n", (uint64_t) *pte_rec_addr);

          *pte_rec_addr = p_addr | PAGE_USER;
          return 1;

        }
        
      } else {
      

        uint64_t physical_page = get_next_free_page_kmalloc();

        *pgd_rec_addr = physical_page | PAGE_USER;

        *pte_rec_addr = p_addr | PAGE_USER;

        return 1;
        
      }
      
    } else {
    

      uint64_t physical_page = get_next_free_page_kmalloc();
      *pdpe_rec_addr = physical_page | PAGE_USER;
      physical_page = get_next_free_page_kmalloc();
      *pgd_rec_addr = physical_page | PAGE_USER;

      *pte_rec_addr = p_addr | PAGE_USER;
      return 1;

    }
    
  } else {
    
    kprintf("2. not valid pml4: %p\n", (uint64_t) *pml4_rec_addr);

    uint64_t physical_page = get_next_free_page_kmalloc();

    // kprintf("new pdpe = %p\n", physical_page);

    *pml4_rec_addr = physical_page | PAGE_USER;

    physical_page = get_next_free_page_kmalloc();

    // kprintf("new pgd = %p, *pml4_rec_addr= %p\n", physical_page, *pml4_rec_addr);

    *pdpe_rec_addr = physical_page | PAGE_USER;

    physical_page = get_next_free_page_kmalloc();

    // kprintf("new pt = %p\n", physical_page);

    *pgd_rec_addr = physical_page | PAGE_USER;

    *pte_rec_addr = p_addr | PAGE_USER;

    // kprintf("*pte_rec_addr = %p\n", *pte_rec_addr);
     // kprintf("pml4_rec_addr = %p, *pml4_rec_addr = %p\n", (uint64_t)pml4_rec_addr, *pml4_rec_addr);
     // kprintf("pdpe_rec_addr = %p, *pdpe_rec_addr = %p\n", (uint64_t)pdpe_rec_addr, *pdpe_rec_addr);
     // kprintf("pgd_rec_addr = %p, *pgd_rec_addr = %p\n", (uint64_t)pgd_rec_addr, *pgd_rec_addr);
     // kprintf("pte_rec_addr = %p, *pte_rec_addr = %p\n", (uint64_t)pte_rec_addr, *pte_rec_addr);

    return 1;

  }

}


uint64_t create_new_kernel_pml4(uint64_t p_addr, uint64_t* pml4_t) {
  
  uint64_t top = top_virtual_address;
  uint64_t* v_addr = (uint64_t *)top;
  

  int result = page_walk(p_addr, (uint64_t)v_addr);

  if (result == 0) {
    kprintf("kmalloc failed!\n");
    return 0;
  }
  top_virtual_address += (uint64_t)PAGESIZE;
  
  v_addr[511] = kernel_pml4_t[511];
  v_addr[510] = p_addr | PAGE_KERNEL;
  

  return (uint64_t)v_addr;

}

uint64_t create_new_user_pml4(uint64_t p_addr, uint64_t* pml4_t) {
  
  uint64_t top = top_virtual_address;
  uint64_t* v_addr = (uint64_t *)top;
  

  int result = page_walk_user(p_addr, (uint64_t)v_addr);

  if (result == 0) {
    kprintf("kmalloc failed!\n");
    return 0;
  }
  top_virtual_address += (uint64_t)PAGESIZE;
  
  v_addr[511] = kernel_pml4_t[511];
  v_addr[510] = p_addr | PAGE_USER;
  

  return (uint64_t)v_addr;

}

void map_kernel_pages(uint64_t v_addr_start, uint64_t v_addr_end, uint64_t p_addr, pml4 *kernel_pml4) {
  
  uint64_t current_v_addr, current_p_addr;
//  int count = 0;
  
  for(current_v_addr = v_addr_start, current_p_addr = p_addr; current_v_addr < (v_addr_start + 10*(uint64_t)PAGESIZE); current_v_addr += (uint64_t)PAGESIZE, current_p_addr += (uint64_t)PAGESIZE) {
    map_kernel_page(current_v_addr, current_p_addr, kernel_pml4);
//    kprintf("mapped 1 page\ncurrent_v_addr = %p\n", current_v_addr);
  }
//  kprintf("count = %d\n", count);
  
}


void map_kernel_page(uint64_t v_addr, uint64_t p_addr, pml4 *kernel_pml4) {
  
//  int count = 1;
  uint64_t pml4_offset = (v_addr >> 39) & 0x1ff;
  uint64_t pdpt_offset = (v_addr >> 30) & 0x1ff;
  uint64_t pgd_offset = (v_addr >> 21) & 0x1ff;
  uint64_t pgt_offset = (v_addr >> 12) & 0x1ff;
//  uint64_t offset = (v_addr >> 0) & 0xfff;
  
  uint64_t *offset_in_pml4_t = (uint64_t *) ((uint64_t)kernel_pml4 + pml4_offset);
//  kprintf("kernel_pml4 = %p, pml4_offset = %p\n", kernel_pml4, pml4_offset);
//  kprintf("offset_in_pml4_t =  %p\n", (uint64_t *) ((uint64_t)kernel_pml4 + pml4_offset));
  
//  uint64_t pdp_table = ((uint64_t)offset_in_pml4_t + KERNBASE);
//  uint64_t *offset_in_pdp_t = (uint64_t *)pdp_table + pdpt_offset;
  
//  kprintf("pdp_table = %p, pdpt_offset = %p\n", pdp_table, pdpt_offset);
//  kprintf("*offset_in_pdp_t = %p\n", (uint64_t *)pdp_table + pdpt_offset);
//  kprintf("original offset_in_pdp_t = %p\n", (uint64_t *)(pdp_table + pdpt_offset));
  
//  return;
  
  

  // if pdp is present
  if(is_valid_page((uint64_t)offset_in_pml4_t)) {
    kprintf("pdp valid\n");
    uint64_t pdp_table = ((uint64_t)offset_in_pml4_t + KERNBASE);
    uint64_t *offset_in_pdp_t = (uint64_t *)(pdp_table + pdpt_offset);
    
    // if pgd is present
    if(is_valid_page((uint64_t)offset_in_pdp_t)) {
      uint64_t pgd_table = ((uint64_t)offset_in_pdp_t + KERNBASE);
      uint64_t *offset_in_pgd_t = (uint64_t *)(pgd_table + pgd_offset);
      
      // if page table is present
      if(is_valid_page((uint64_t)offset_in_pgd_t)) {
        uint64_t pg_table = ((uint64_t)offset_in_pgd_t + KERNBASE);
        uint64_t *offset_in_pg_t = (uint64_t *)(pg_table + pgt_offset);
        
        
        *offset_in_pg_t = p_addr | PAGE_KERNEL;
//        kprintf("virtual = %p, physical = %p\n", offset_in_pg_t, p_addr);

//        count = 0;
        
      }
      else {

        // create page table and offset in pgd
        uint64_t pg_table_physical = get_next_free_page();
        uint64_t pg_table = pg_table_physical + KERNBASE;
        *offset_in_pgd_t = pg_table | PAGE_KERNEL;

        // this is the physical page mapping
        uint64_t *offset_in_pg_t = (uint64_t *)(pg_table + pgt_offset);

        *offset_in_pg_t = p_addr | PAGE_KERNEL;
//        kprintf("virtual = %p, physical = %p\n", offset_in_pg_t, p_addr);

      }
    }
    else {

      // create pgd table and entry in pdp
      uint64_t pgd_table_physical = get_next_free_page();
      uint64_t pgd_table = pgd_table_physical + KERNBASE;
      *offset_in_pdp_t = pgd_table | PAGE_KERNEL;

      // create page table and offset in pgd
      uint64_t pg_table_physical = get_next_free_page();
      uint64_t pg_table = pg_table_physical + KERNBASE;
      uint64_t *offset_in_pgd_t = (uint64_t *) (pgd_table + pgd_offset);
      *offset_in_pgd_t = pg_table | PAGE_KERNEL;

      // this is the physical page mapping
      uint64_t *offset_in_pg_t = (uint64_t *)(pg_table + pgt_offset);

      *offset_in_pg_t = p_addr | PAGE_KERNEL;
      
//      kprintf("virtual = %p, physical = %p\n", offset_in_pg_t, p_addr);

    }
  }
  else {
//    kprintf("pdp not valid\n");
    // create pdp table and entry in pml4
    uint64_t pdp_table_physical = get_next_free_page();
    uint64_t pdp_table = (pdp_table_physical + KERNBASE);
//    *offset_in_pml4_t = pdp_table | PAGE_KERNEL;
//    kprintf("offset_in_pml4_t = %p\n", offset_in_pml4_t);
    
//    kprintf("created offset_in_pml4_t\n");
    
    // create pgd table and entry in pdp
    uint64_t pgd_table_physical = get_next_free_page();
    uint64_t pgd_table = pgd_table_physical + KERNBASE;
    uint64_t *offset_in_pdp_t = (uint64_t *) (pdp_table + pdpt_offset);
    *offset_in_pdp_t = pgd_table | PAGE_KERNEL;
    
//    kprintf("created pdg table\n");
    
    // create page table and offset in pgd
    uint64_t pg_table_physical = get_next_free_page();
    uint64_t pg_table = pg_table_physical + KERNBASE;
    uint64_t *offset_in_pgd_t = (uint64_t *)(pgd_table + pgd_offset);
    *offset_in_pgd_t = pg_table | PAGE_KERNEL;
    
//    kprintf("created the page table\n");

    // this is the physical page mapping
    uint64_t *offset_in_pg_t = (uint64_t *)(pg_table + pgt_offset);
    
//    kprintf("created offset in pg table\n");

    *offset_in_pg_t = p_addr | PAGE_KERNEL;
//    kprintf("virtual = %p, physical = %p\n", offset_in_pg_t, p_addr);

//    kprintf("wrote to offset in page table\n");
//    return;

  }
  
//  return count;
  
}



int is_valid_page(uint64_t page_address) {
  if ((page_address & 0x03) == 0x03) {
    return 1;
  }
  return 0;
}

/*
OLD IMPLEMENTATIONS:
*/

// kmalloc old implementation
uint64_t kmalloc_old(uint64_t size){
  uint64_t flh_va = KERNBASE + (uint64_t)free_list_head;
  Page *temp = (Page *)flh_va;
  uint64_t ret_addr = KERNBASE + temp->p_addr;

  uint64_t num_pages;
  if(size % PAGESIZE == 0){
    num_pages = size/PAGESIZE;
  }else{
    num_pages = size/PAGESIZE + 1;
  }
  while(num_pages--){
    temp->status = 1;
    temp = (Page *)(KERNBASE + (uint64_t)temp->next);
  }
  free_list_head = (Page*)((uint64_t)temp - KERNBASE);
  
  //kprintf("New flh is : %p\n", free_list_head);
  return ret_addr;
}

// page fault handler old implementation
void page_fault_handler() {
  kprintf("page fault\n");
  uint64_t current_cr2 = get_cr2();
  kprintf("cr2 = %p\n", current_cr2);
  return;
}

// prints the corresponding physical address for given virtual address
void print_va_to_pa(uint64_t vaddr, pml4* pml4_t) {
  pdp *pdpt;
  pd *pd_t;
  pt *pt_t;
  uint64_t *pga;
//  uint64_t paddr;
  
  uint64_t pml4off = get_offset(vaddr, PML4SHIFT);
  uint64_t pdpoff = get_offset(vaddr, PDPSHIFT);
  uint64_t pdoff = get_offset(vaddr, PDSHIFT);
  uint64_t ptoff = get_offset(vaddr, PTSHIFT);
  kprintf("pml4off = %p, %p, %p, %p\n", pml4off, pdpoff, pdoff, ptoff);
  pdpt = (pdp *)(*(pml4_t + pml4off) & (uint64_t)PHYMASK);
  kprintf("Va :%p and pdpt :%p\n", vaddr, pdpt);
  pd_t = (uint64_t *) (((uint64_t *)to_virtual((uint64_t)pdpt))[pdpoff] & (uint64_t)PHYMASK);
//  pd_t = (pd *)(*(pdpt + pdpoff) & PHYMASK);
  kprintf("Va :%p and pd_t :%p\n", vaddr, pd_t);
  pt_t = (uint64_t *) (((uint64_t *)to_virtual((uint64_t)pd_t))[pdoff] & (uint64_t)PHYMASK);
//  pt_t = (pt *)(*(pd_t + pdoff) & PHYMASK);
  kprintf("Va :%p and pt_t :%p\n", vaddr, pt_t);
  pga = (uint64_t *) (((uint64_t *)to_virtual((uint64_t)pt_t))[ptoff] & (uint64_t)PHYMASK);
//  pga = (uint64_t)(*(pt_t + ptoff) & PHYMASK);
  kprintf("Va :%p and pga :%p\n", vaddr, pga);
//  paddr = ((uint64_t) *(pga + vaddr)) & PAMASK;
//  paddr = pga + (vaddr & PAMASK);
//  kprintf("Va :%p and pa :%p\n", vaddr, paddr);
}

uint64_t get_offset(uint64_t addr, uint64_t shift){
  return (addr >> shift) & 0x1FF;
}
