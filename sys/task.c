#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/elf64.h>
#include <sys/paging.h>

void kthread1(){
  kprintf("In thread 1\n");
}

/*movq label, %%rax;\
pushq %%rax;\*/

// context switching:
void switch_to(pcb *p1, pcb *p2){
  __asm__ volatile(
    "\
      pushq $1f;\
      pushq %%rsi;\
      pushq %%rdi;\
      pushq %%rbp;\
      pushq %%rax;\
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
      pushq %%r15;\
    "
    :
    :
  );
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
      popq %%r15;\
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
      popq %%rax;\
      popq %%rbp;\
      popq %%rdi;\
      popq %%rsi;\
      retq;\
    "
    :
    :
    :"%rbp","%rdi","%rsi","%rax","%rbx","%rcx","%rdx","%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15"
  );
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
