#ifndef __TASK_H
#define __TASK_H
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/mm.h>

typedef struct pcb{
  uint64_t kstack[128];
  uint64_t pid;
  uint64_t rsp;
  uint64_t cr3;
  struct mm_struct mm;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
}pcb;

void kthread1();
void switch_to(pcb*, pcb*);

#endif
