#ifndef __TASK_H
#define __TASK_H
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/mm.h>

typedef struct pcb{
  uint64_t kstack[512];
  uint64_t *pstack;
  uint64_t pid;
  uint64_t rsp;
  uint64_t cr3;
  struct mm_struct *mm;
  enum run_state{RUNNING, SLEEPING, ZOMBIE, IDLE} state;
  int exit_status;
  struct pcb *next;
  uint64_t e_entry;
  uint64_t time_remaining;
  enum p_type{USER, KERNEL} process_type;
}pcb;

void kthread1();
void switch_to(pcb*, pcb*);
void first_user_process(pml4 *);
void switch_to_user_mode(pcb*);
pcb *create_process(struct mm_struct *head);
void first_process_switch(pcb *process, uint64_t rip, uint64_t flags);
pcb *create_dummy_process();

#endif
