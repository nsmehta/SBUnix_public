#ifndef __TASK_H
#define __TASK_H
#include<sys/defs.h>
#include<sys/kprintf.h>

typedef struct pcb{
  uint64_t kstack[128];
  uint64_t pid;
  uint64_t rsp;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
}pcb;

void kthread1();
void switch_to(pcb*, pcb*);

#endif
