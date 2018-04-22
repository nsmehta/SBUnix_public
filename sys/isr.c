#include <sys/isr.h>
#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/io.h>
#include <sys/keyboard.h>
#include <sys/paging.h>

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
        kprintf("%s", exception_messages[r.int_no]);
        if (r.int_no == 13) {
          kprintf("cr2 = %p\n", get_cr2());
        }
        kprintf(" Exception. System Halted!\n");

        //for (;;);
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
