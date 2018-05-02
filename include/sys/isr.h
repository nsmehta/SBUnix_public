#ifndef _ISR_H
#define _ISR_H

#include <sys/defs.h>
#include <sys/paging.h>
#include <sys/task.h>

typedef struct {
    // uint64_t ds;
    // uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rbp, rdx, rcx, rbx, rax, rdi; //  rdx, rcx, rbx, rax, rbp, rdi, rsi;
    uint64_t int_no, err_code;
    // uint64_t rip, cs, rflags, useresp, ss;  // Pushed by the processor automatically
    // uint64_t rip, cs, rflags, useresp, ss;  // Pushed by the processor automatically
}__attribute__((packed)) registers;

uint64_t first_schedule;

void isr_install();
void fault_handler(registers );

int timer_ticks;

void timer_phase(int frequency);

void generate_timer();

void user_mode_function();

pcb *get_next_ready_process();

void schedule_old_process(pcb *process);

void syscall_handler(registers *);

#endif
