#include<sys/task.h>
#include<sys/kprintf.h>

void kthread1(){
  kprintf("In thread 1\n");
}

/*movq label, %%rax;\
pushq %%rax;\*/

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
