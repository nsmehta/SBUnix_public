#include <sys/isr.h>
#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/io.h>
#include <sys/keyboard.h>
#include <sys/paging.h>
#include <sys/timer.h>
#include <sys/task.h>
#include <sys/gdt.h>
#include <sys/schedule.h>

/* from http://www.osdever.net/bkerndev/Docs/isrs.htm */

/*ISR prototypes*/
// extern void isr0();
// extern void isr1();
// extern void isr2();
// extern void isr3();
// extern void isr4();
// extern void isr5();
// extern void isr6();
// extern void isr7();
// extern void isr8();
// extern void isr9();
// extern void isr10();
// extern void isr11();
// extern void isr12();
// extern void isr13();
// extern void isr14();
// extern void isr15();
// extern void isr16();
// extern void isr17();
// extern void isr18();
// extern void isr19();
// extern void isr20();
// extern void isr21();
// extern void isr22();
// extern void isr23();
// extern void isr24();
// extern void isr25();
// extern void isr26();
// extern void isr27();
// extern void isr28();
// extern void isr29();
// extern void isr30();
// extern void isr31();

void isr_install() {

  //idt_set_gate:

  idt_set_gate(0, (uint64_t)&isr0, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(1, (uint64_t)&isr1, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(2, (uint64_t)&isr2, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(3, (uint64_t)&isr3, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(4, (uint64_t)&isr4, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(5, (uint64_t)&isr5, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(6, (uint64_t)&isr6, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(7, (uint64_t)&isr7, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(8, (uint64_t)&isr8, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(9, (uint64_t)&isr9, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(10, (uint64_t)&isr10, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(11, (uint64_t)&isr11, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(12, (uint64_t)&isr12, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(13, (uint64_t)&isr13, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(14, (uint64_t)&isr14, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(15, (uint64_t)&isr15, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(16, (uint64_t)&isr16, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(17, (uint64_t)&isr17, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(18, (uint64_t)&isr18, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(19, (uint64_t)&isr19, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(20, (uint64_t)&isr20, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(21, (uint64_t)&isr21, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(22, (uint64_t)&isr22, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(23, (uint64_t)&isr23, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(24, (uint64_t)&isr24, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(25, (uint64_t)&isr25, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(26, (uint64_t)&isr26, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(27, (uint64_t)&isr27, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(28, (uint64_t)&isr28, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(29, (uint64_t)&isr29, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(30, (uint64_t)&isr30, 0x08, 0, 0x0e, 0, 1);
  idt_set_gate(31, (uint64_t)&isr31, 0x08, 0, 0x0e, 0, 1);


}

char *exception_messages[] = {
  "Divide by Zero Exception",
  "Debug Exception",
  "NMI Exception",
  "Breakpoint Exception",
  "Into Detected Overflow Exception",
  "Out of bounds Exception",
  "Invalid Opcode Exception",
  "No Coprocessor Exception",
  "Double Fault Exception",
  "Coprocessor Segment Overrun Exception",
  "Bad TSS Exception",
  "Segment Not Present Exception",
  "Stack Fault Exception",
  "General Protection Fault Exception",
  "Page Fault Exception",
  "Unknown Interrupt Exception",
  "Coprocessor Fault Exception",
  "Alignment Check Exception (486+)",
  "Machine Check Exception (Pentium/586+)",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions",
  "Reserved Exceptions"
};

void fault_handler(registers r)
{
    kprintf("in fault handler\n");
    // __asm__ __volatile__("iretq");
    kprintf("r.int_no = %d\n", r.int_no);
    if (r.int_no == 14) {
      page_fault_handler();
    }
    
    else if (r.int_no < 32)
    {
        kprintf("%s\n", exception_messages[r.int_no]);
        if (r.int_no == 13) {
          kprintf("cr2 = %p\n", get_cr2());
        }
        kprintf(" Exception. System Halted!\n");

        for (;;);
	}
    //for (;;);
    outb(0x20, 0x20);
}

void irq_handler(registers r)
{
		if(r.int_no == 32) {
			outb(0x20, 0x20);
			return;
		}

    if (r.int_no >= 40)
    {

        kprintf(" Exception. System Halted!\n");

        // send reset signal to slave
        outb(0xA0, 0x20);
    }
		if(r.int_no == 33) {
			keyboard_interrupt();
		}

    // send reset signal to master
    outb(0x20, 0x20);
}

/* source: http://www.osdever.net/bkerndev/Docs/pit.htm */
void timer_phase(int frequency) {
  
  int divisor = 1193180 / frequency;
  outb(0x43, 0x36);
  outb(0x40, divisor & 0xff);
  outb(0x40, divisor >> 8);
  
  timer_ticks = 0;
  
}

void user_mode_function() {
  while(1) {
   kprintf("in user mode\n"); 
  }
}


void generate_timer() {
  
  timer_ticks++;
//  kprintf("scene kya hai ?\n");
  if (timer_ticks == 180) {
    kprintf("timer !\n");
    timer_ticks = 0;
    
    if(ready_processes == NULL) {
      
      // run idle process:
//      uint64_t old_cr3 = get_cr3();
//      kprintf("old cr3 = %p\n", old_cr3);
      uint64_t *old_rsp;
//      uint64_t *new_rsp = (uint64_t *)idle_process->rsp;
//      kprintf("new rsp = %p\n", new_rsp);
      
      __asm__ volatile (
        "movq %%rsp, %0;"
        :"=r"(old_rsp)
        :
      );
      
      
//      kprintf("old rsp = %p\n", old_rsp);
//      kprintf("before set cr3\n");
      set_cr3((uint64_t *)idle_process->cr3);
//      kprintf("after set cr3:  %p\n",get_cr3());
      
      //set new rsp
      __asm__ volatile(
        "movq %0, %%rsp;"
        :
        : "m" (idle_process->rsp)
      );
      ready_processes = idle_process;
      
//      kprintf("after load rsp\n");
    }
  /*
  
  struct pcb *process = (struct pcb *)kmalloc(sizeof(struct pcb));
  kprintf("first process = %p\n", (uint64_t)process);
  
  uint64_t flags = 0x0;
  
  __asm__ volatile(
  "\
    pushfq;\
    popq  %0;\
  "
  :"=r" (flags)
  :
  );
  
  kprintf("flags = %p\n", flags);

  
  
  uint64_t rip = (uint64_t)&user_mode_function;
  process->pstack = (uint64_t *) kmalloc(PAGESIZE);
  
  process->kstack[127] = 0x23;
  process->kstack[126] = process->pstack[511];
  process->kstack[125] = flags | 0x200;
  process->kstack[124] = 0x2b;
  process->kstack[123] = rip;
  
  process->kstack[123 - 15] = (uint64_t)irq0 + 0x20;
  
  process->rsp = (uint64_t) &process->kstack[123 - 16];
  
  process->cr3 = get_next_free_page_kmalloc();
  kprintf("cr3 = %p\n", process->cr3);
  
  create_new_user_pml4(process->cr3, kernel_pml4_t);
  
  // load the new process's cr3
  set_cr3((uint64_t *)process->cr3);
  
  // set new process's rsp
  __asm__ volatile(
  "\
    movq %0, %%rsp;\
  "
  :
  : "r" (process->rsp)
  );
  
  set_tss_rsp((void *)((uint64_t)&process->kstack[127]));
  
  __asm__ volatile(
  "\
   movq $0x23, %rax;\
   movq %rax, %ds;\
   movq %rax, %es;\
   movq %rax, %fs;\
   movq %rax, %gs;\
  "
  );

  */
  }
  
  // send reset signal to master
  outb(0x20, 0x20);

}
