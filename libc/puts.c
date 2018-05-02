#include <stdio.h>
#include <syscall.h>
#include <stdarg.h>
#include <string.h>
#include <sys/defs.h>

int puts(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
}


int printf(const char *fmt, ...) {
	va_list parameters;
	va_start(parameters, fmt);

	char buffer[2000];
	strcpy((char *)buffer, (char *)fmt);
	va_end(parameters);

	// syscall_2_parameters(1, (uint64_t)&buffer);
	syscall_3_parameters(1, (uint64_t)&buffer, (uint64_t)&buffer);
	return 0;
}
