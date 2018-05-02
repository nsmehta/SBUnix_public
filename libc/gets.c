#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>

char *gets(char *s) {

	// syscall_2_parameters(1, (uint64_t)&buffer);
	syscall_2_parameters(2, (uint64_t)s);

	return s;
}