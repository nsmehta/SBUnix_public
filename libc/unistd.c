#include <unistd.h>
#include <string.h>
#include <syscall.h>

char *getcwd(char *buf, size_t size) {
	//getcwd
	char *s = '\0';
	syscall_2_parameters(5, (uint64_t)s);
	return s;
}