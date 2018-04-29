#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/elf64.h>
#include <sys/paging.h>
#include <sys/schedule.h>


extern void irq0();

void scheduler_init() {
  
  ready_processes = NULL;
  sleeping_processes = NULL;
  idle_process = NULL;
  
}

void add_ready_process(struct pcb *process) {
  
  if (ready_processes == NULL) {
    
    ready_processes = process;
    ready_processes->next = NULL;
    
  } else {
    
    struct pcb *temp = ready_processes;
    while (temp->next != NULL) {
      
      temp = temp->next;
      
    }
    temp->next = process;
    process->next = NULL;
    
  }
  
}

void add_sleeping_process(struct pcb *process) {
  
  if (sleeping_processes == NULL) {
    
    sleeping_processes = process;
    sleeping_processes->next = NULL;
    
  } else {
    
    struct pcb *temp = sleeping_processes;
    while (temp->next != NULL) {
      
      temp = temp->next;
      
    }
    temp->next = process;
    process->next = NULL;
    
  }
  
}

struct pcb * get_next_ready_process() {
  
  if (ready_processes == NULL) {
    
    return NULL;
    
  } else {
    
    struct pcb *temp = ready_processes;
    ready_processes = ready_processes->next;
    return temp;
  }
  
}

void schedule_idle_process(pcb *dummy_process, uint64_t function_pointer) {
  
  // setup dummy process stack:
  
  dummy_process->kstack[511] = 0x10; // SS
  dummy_process->kstack[510] = (uint64_t) &(dummy_process->kstack[511]);
  dummy_process->kstack[509] = 0x200202;
  dummy_process->kstack[508] = 0x08;
  dummy_process->kstack[507] = function_pointer;
  
  dummy_process->kstack[507 - 16] = (uint64_t)irq0 + 0x20;
  dummy_process->rsp = (uint64_t) &(dummy_process->kstack[507 - 17]);
  
  idle_process = dummy_process;
  
  kprintf("idle_process->rsp = %p\n", idle_process->rsp);
  
}


void schedule_process(pcb *process) {
  
  
  
}

