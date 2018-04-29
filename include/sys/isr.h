#ifndef _ISR_H
#define _ISR_H

#include <sys/defs.h>

typedef struct {
    uint64_t ds;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, useresp, ss;  // Pushed by the processor automatically.
} registers;

void isr_install();
void fault_handler(registers );

int timer_ticks;

void timer_phase(int frequency);

void generate_timer();

void user_mode_function();

#endif
