#include <sys/mm.h>
#include <sys/kprintf.h>

void memset(void *ptr, int val, uint64_t size){
  char *p = (char *)ptr;
  while(size--){
    *p = val;
    p++;
  }
}

void ring_3_switch(uint64_t user_rip) {

  uint64_t user_rsp = 0x4000c0;
  __asm__ volatile(
"cli;"
"movq $0x23, %%rax;"
"movq %%rax, %%ds;"
"movq %%rax, %%es;"
"movq %%rax, %%fs;"
"movq %%rax, %%gs;"
//"movq %%rsp, %%rax;"
"pushq $0x23;"
"pushq %0;"
"pushfq;"
"popq %%rax;"
"or $0x200, %%rax;"
"pushq %%rax;"
"pushq $0x1b;"
"pushq %1;"
"iretq;"
//"1:"
//:
//:
//		   );
:
:"r"(user_rsp), "r"(user_rip)
:"memory", "rax"
		   );
 
  kprintf("failed to go to user mode!\n");

  
}
