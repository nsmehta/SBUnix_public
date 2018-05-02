#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>

uint64_t syscall_2_parameters(uint64_t syscall_no, uint64_t p1);
uint64_t syscall_3_parameters(uint64_t syscall_no, uint64_t p1, uint64_t p2);
uint64_t syscall_4_parameters(uint64_t syscall_no, uint64_t p1, uint64_t p2, uint64_t p3);

#endif