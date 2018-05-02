#include <stdio.h>
#include <sys/defs.h>

int putchar(int c)
{
  // write character to stdout
	uint64_t ch_int = (uint64_t) c;
	// char ch = (char);
	char *input = (char *)ch_int;
	return printf(input);
}
