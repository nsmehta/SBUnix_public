#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include <sys/defs.h>
#include <sys/mm.h>
#include <sys/task.h>

struct pcb *ready_processes;
struct pcb *sleeping_processes;
struct pcb *idle_process;
struct pcb *currently_running_process;

uint64_t pid_count;


void scheduler_init();
void add_ready_process(struct pcb *process);
void add_sleeping_process(struct pcb *process);
//struct pcb * get_next_ready_process();
void schedule_idle_process(pcb *dummy_process, uint64_t function_pointer);
void schedule_user_process(pcb *process);
void schedule_ready_process(pcb *dummy_process, uint64_t function_pointer);

#endif