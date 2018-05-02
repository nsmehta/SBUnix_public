#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <syscall.h>

// int execvpe(const char *file, char *const argv[], char *const envp[]) {

// 	return syscall_4_parameters(3, (uint64_t)file, (uint64_t)argv, (uint64_t)envp);

// }