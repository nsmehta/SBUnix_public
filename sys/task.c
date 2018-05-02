#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/elf64.h>
#include <sys/paging.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <sys/schedule.h>


extern void irq0();

void kthread1(){
  kprintf("In thread 1\n");
}

/*movq label, %%rax;\
pushq %%rax;\*/

/*source: http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html*/

void switch_to_user_mode(pcb *p1) {

  __asm__ volatile(
    "\
      cli;\
      movq $0x23, %%rax;\
      movq %%rax, %%ds;\
      movq %%rax, %%es;\
      movq %%rax, %%fs;\
      movq %%rax, %%gs;\
      movq %%rsp, %%rax;\
      pushq $0x23;\
      pushq %%rax;\
      pushfq;\
      popq %%rax;\
      or $0x200, %%rax;\
      pushq %%rax;\
      pushq $0x2b;\
      pushq %0;\
      iretq;\
    "
    :
    :"m" (p1->rsp)
  );
  
  /*__asm__ volatile(
    "\
      movq %0, %%rsp\
    "
    :
    :"m" (p1->rsp)
  );*/
  kprintf("Value in rsp p1 is %p\n", p1->rsp);

/*  __asm__ volatile(
    "\
      iretq;\
    "
  );*/

  __asm__ volatile("2:");

  
}

// context switching:
void switch_to(pcb *p1, pcb *p2){
  
  __asm__ volatile(
    "\
      pushq $1f;\
    "
  );
  
//  __asm__ volatile(
/*    "\
//      pushq $1f;\
//      pushq %%rsi;\
//      pushq %%rdi;\
//      pushq %%rbp;\
//      pushq %%rax;\
//      pushq %%rbx;\
//      pushq %%rcx;\
//      pushq %%rdx;\
//      pushq %%r8;\
//      pushq %%r9;\
//      pushq %%r10;\
//      pushq %%r11;\
//      pushq %%r12;\
//      pushq %%r13;\
//      pushq %%r14;\
//      pushq %%r15;\
//    "
//    :
//    :
//  );*/
  __asm__ volatile(
    "\
      movq %%rsp, %0\
    "
    :
    :"m" (p1->rsp)
  );
  kprintf("Value in rsp p1 is %p\n", p1->rsp);
  kprintf("value in rsp p2 is %p\n", p2->rsp);
  __asm__ volatile(
    "\
      movq %0, %%rsp\
    "
    :
    :"m" (p2->rsp)
  );
  __asm__ volatile(
    "\
      retq;\
    "
  );
/*  __asm__ volatile(
//    "\
//      popq %%r15;\
//      popq %%r14;\
//      popq %%r13;\
//      popq %%r12;\
//      popq %%r11;\
//      popq %%r10;\
//      popq %%r9;\
//      popq %%r8;\
//      popq %%rdx;\
//      popq %%rcx;\
//      popq %%rbx;\
//      popq %%rax;\
//      popq %%rbp;\
//      popq %%rdi;\
//      popq %%rsi;\
//      retq;\
//    "
//    :
//    :
//    :"%rbp","%rdi","%rsi","%rax","%rbx","%rcx","%rdx","%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15"
//  );*/
  __asm__ volatile("1:");

}

/*pushq %%rax;\
pushq %%rbx;\
pushq %%rcx;\
pushq %%rdx;\
pushq %%r8;\
pushq %%r9;\
pushq %%r10;\
pushq %%r11;\
pushq %%r12;\
pushq %%r13;\
pushq %%r14;\
pushq %%r15;\*/

/*popq %%r15;\
popq %%r14;\
popq %%r13;\
popq %%r12;\
popq %%r11;\
popq %%r10;\
popq %%r9;\
popq %%r8;\
popq %%rdx;\
popq %%rcx;\
popq %%rbx;\
popq %%rax;\*/


void first_user_process(pml4 *kernel_pml4_t){

  char *filename = "bin/sbush";
  Elf64_Ehdr *p = get_elf(filename);
  kprintf("\nreturned to main %x%c\n", p->e_ident[0], p->e_ident[1]);
  int result = validate_elf_header(p);
  kprintf("result = %d\n", result);
  result = check_elf_loadable(p);
  kprintf("elf loadable = %d\n", result);
  struct mm_struct *head = NULL;
  if(result) {
    head = load_elf_vmas(p);
  }
  kprintf("mm_struct = %p\n", head->mmap->vm_start);
  int count = 0;
  struct vm_area_struct *temp = head->mmap;
  while(temp != NULL){
    kprintf("vm_start = %d\n", temp->vm_type);
    temp = temp->vm_next;
    count++;
  }
  kprintf("count = %d\n", count);
  void *q = (void *)p->e_entry;
  kprintf("value at start address = %p\n", q);

  pcb *process_1 = (pcb *)kmalloc(sizeof(pcb));
  process_1->mm = head;
  process_1->pid = 1;

  /*  pml4 *process_pml4 = (pml4 *)kmalloc(sizeof(pml4)*512);
  process_pml4_t[511] = kernel_pml4_t[511];

  uint64_t pml4_phy_add = process_pml4 - KERNBASE;
  process_pml4_t[510] = pml4_phy_add | 0x7;
  */
  
}


struct pcb *create_process(struct mm_struct *head) {
  
  kprintf("in create_process\n");
  struct pcb* new_process = (struct pcb *)kmalloc_user(sizeof(pcb));
  
  memset((void *)new_process, 0, (uint64_t)sizeof(pcb) / (uint64_t)8);
  
  uint64_t process_cr3 = get_next_free_page_kmalloc();
  
  create_new_user_pml4(process_cr3, kernel_pml4_t);
  
  uint64_t old_cr3 = get_cr3();

  new_process->cr3 = process_cr3;
  
  new_process->e_entry = head->e_entry;

  kprintf("new_process->e_entry = %p\n", new_process->e_entry);
  
  struct vm_area_struct *temp = head->mmap;
  while(temp != NULL){
    if (temp->vm_type == TYPE_FILE) {
      // kprintf("process_cr3 = %p\n", process_cr3);
      // uint64_t padd = get_next_free_page_kmalloc();
      // kprintf("get next free page kmalloc = %p\n", padd);

      set_cr3((uint64_t *)process_cr3);
      
      uint64_t vm_start = page_align(temp->vm_start);
      uint64_t vm_size = round_up(temp->vm_end - vm_start);
      
      // kprintf("before kmalloc_top_virtual_address_user: %p, %p\n", vm_start, vm_size);
      uint64_t v_addr = kmalloc_top_virtual_address_user(vm_size, vm_start);
      // uint64_t v_addr = kmalloc_user(vm_size);
      // kprintf("after kmalloc_top_virtual_address_user: %p\n", v_addr);
      // kprintf("v_addr[0] = %p\n",((uint64_t *)v_addr)[0]);
      // while(1);
      
      //Copy values from p_offset to the virtual address vm_start in the current paging structure
      uint8_t *address = (uint8_t *)v_addr;
      uint8_t *offset = (uint8_t *)(head->elf_head + temp->vm_offset);

      kprintf("offset = %p\n", offset);
      kprintf("head->elf_head = %p\n", head->elf_head);
      kprintf("temp->vm_offset = %p\n", temp->vm_offset);
      kprintf("vm_size = %p\n", vm_size);
      // while(1);
      
      //Copy values from p_offset to the virtual address vm_start in the current paging structure
      for(uint64_t i = 0; i< vm_size; i++) {
        *address = *offset;
        address++;
        offset++;
      }
      kprintf("*address = %p\n", address);
      address = (uint8_t *)v_addr;

      kprintf("*address = %p\n", address);
      address++;
      kprintf("*address = %p\n", *address);
      address++;
      kprintf("*address = %p\n", *address);
      // while(1);



      set_cr3((uint64_t *)old_cr3);
    } else if (temp->vm_type == TYPE_STACK) {
      set_cr3((uint64_t *)process_cr3);
      kprintf("paging_user_stack_top = %p\n", paging_user_stack_top);
      uint64_t stack_top = paging_user_stack_top;
      // paging_user_stack_top -= (PAGESIZE * 3);
      uint64_t *new_stack = (uint64_t *)kmalloc_top_virtual_address_user(PAGESIZE, stack_top - PAGESIZE + 8);
      kprintf("new_stack = %p\n", new_stack);
      kprintf("paging_user_stack_top = %p\n", paging_user_stack_top);
      new_process->pstack = (uint64_t *)(stack_top - 64);
      kprintf("new_process->pstack = %p, [-100] = %p\n", new_process->pstack, &(((uint64_t *)new_process->pstack)[-100]));
      temp->vm_start = (uint64_t)new_process->pstack;
      temp->vm_end = (uint64_t)(new_process->pstack + (512*3 - 1));
      kprintf("*new_process->pstack = %p\n", ((uint64_t *)new_process->pstack)[-100]);
      set_cr3((uint64_t *)old_cr3);
    }
    kprintf("vm_start = %d\n", temp->vm_type);
    temp = temp->vm_next;
//    count++;
  }

  // kprintf("new process returned\n");
//  uint64_t *new_stack = kmalloc(PAGESIZE);
  return new_process;
  
}


pcb *create_dummy_process() {
  
  kprintf("in dummy process\n");
  pcb* new_process = (pcb *)kmalloc_user(sizeof(pcb));
  
  memset((void *)((uint64_t)new_process), 0, (uint64_t)sizeof(pcb) / (uint64_t)8);
  
  new_process->pstack = NULL;
  new_process->pid = (uint64_t) NULL;
  new_process->rsp = (uint64_t) NULL;
  new_process->cr3 = get_next_free_page_kmalloc();
  create_new_user_pml4(new_process->cr3, kernel_pml4_t);
  new_process->state = RUNNING;
  new_process->exit_status = 0;
  new_process->next = NULL;
  new_process->e_entry = (uint64_t) NULL;
  new_process->time_remaining = (uint64_t) 0x3;
  
  return new_process;
}


void first_process_switch(pcb *process, uint64_t rip, uint64_t flags) {
  
//  process->pstack = (uint64_t *) kmalloc(PAGESIZE);
  
  process->kstack[127] = 0x23; // DS/SS
  process->kstack[126] = (uint64_t) &process->kstack[127]; //RSP
  process->kstack[125] = flags; //flags
  process->kstack[124] = 0x2b; //CS
  process->kstack[123] = rip; //rip
  
  process->kstack[123 - 16] = (uint64_t)irq0 + 32;
  
  process->rsp = (uint64_t) &process->kstack[123 - 17];
  
  // physical address of cr3
  process->cr3 = get_next_free_page_kmalloc();
  
  // map physical cr3 to virtual cr3
  create_new_user_pml4(process->cr3, kernel_pml4_t);
  
  
  // load the new process's cr3
  set_cr3((uint64_t *)process->cr3);
  
  
  
//  process->pstack = (uint64_t *) kmalloc(PAGESIZE);
//  process->kstack[126] = (uint64_t) &process->pstack[511];
  
  // set new process's rsp
  __asm__ volatile(
  "\
    movq %0, %%rsp;\
  "
  :
  : "r" (process->rsp)
  );
  
  
  //set_tss_rsp((void *) &process->kstack[127]);

  kprintf("process->kstack[127] = %p\n", process->kstack[127]);
  kprintf("process->kstack[126] = %p\n", process->kstack[126]);
  kprintf("process->kstack[125] = %p\n", process->kstack[125]);
  kprintf("process->kstack[124] = %p\n", process->kstack[124]);
  kprintf("process->kstack[123] = %p\n", process->kstack[123]);

  
/*  __asm__ volatile(
  "\
   movq $0x23, %rax;\
   movq %rax, %ds;\
   movq %rax, %es;\
   movq %rax, %fs;\
   movq %rax, %gs;\
  "
  );
  */
  
  for(uint64_t i = 0; i < 10000; i++) {
    for(uint64_t j = 0; j < 10000; j++) {
      
    }
  }
  
  kprintf("process->kstack[127] = %p\n", process->kstack[127]);
  kprintf("process->kstack[126] = %p\n", process->kstack[126]);
  kprintf("process->kstack[125] = %p\n", process->kstack[125]);
  kprintf("process->kstack[124] = %p\n", process->kstack[124]);
  kprintf("process->kstack[123] = %p\n", process->kstack[123]);
  
  
  kprintf("here");
//  while(1);

}


int exec_new_binary(char *filename) {

  Elf64_Ehdr *p = get_elf(filename);
  if (p == NULL) {
    return 0;
  }
  kprintf("\nreturned to main %x%c\n", p->e_ident[0], p->e_ident[1]);
  int result = validate_elf_header(p);
  kprintf("result = %d\n", result);
  result = check_elf_loadable(p);
  kprintf("elf loadable = %d\n", result);
  struct mm_struct *head = NULL;
  if(result) {
    head = load_elf_vmas(p);
  }
  kprintf("mm_struct = %p\n", head->mmap->vm_start);
  int count = 0;
  struct vm_area_struct *temp = head->mmap;
  while(temp != NULL){
    kprintf("vm_start = %d\n", temp->vm_type);
    temp = temp->vm_next;
    count++;
  }
  kprintf("count = %d\n", count);
  kprintf("ls binary");


  struct pcb *first_process = create_process(head);
  kprintf("first process = %p\n", (uint64_t)first_process);

  schedule_user_process(first_process);

  return 1;
}

