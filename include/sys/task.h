#ifndef __TASK_H
#define __TASK_H
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/mm.h>

typedef struct pcb{
  uint64_t kstack[128];
  uint64_t *pstack;
  uint64_t pid;
  uint64_t rsp;
  uint64_t cr3;
  struct mm_struct *mm;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
  struct pcb *next;
  uint64_t e_entry;
}pcb;

void kthread1();
void switch_to(pcb*, pcb*);
void first_user_process(pml4 *);
void switch_to_user_mode(pcb*);
pcb *create_process(struct mm_struct *head);
void first_process_switch(pcb *process, uint64_t rip, uint64_t flags);
pcb *create_dummy_process();

#endif
