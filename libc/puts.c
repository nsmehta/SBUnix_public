#include <stdio.h>
#include <syscall.h>
#include <stdarg.h>
#include <string.h>
#include <sys/defs.h>
#include <math.h>

int puts(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
}


// int printf(const char *fmt, ...) {
// 	va_list parameters;
// 	va_start(parameters, fmt);

// 	char buffer[2000];
// 	strcpy((char *)buffer, (char *)fmt);
// 	va_end(parameters);

// 	syscall_2_parameters(1, (uint64_t)&buffer);
// 	// syscall_3_parameters(1, (uint64_t)&buffer, (uint64_t)&buffer);
// 	return 0;
// }

int printPointer(volatile char *current, uint64_t num) {
	if(num == 0) {
		return 0;
	}
	int counter = printPointer(current, num / 16);
	int digit = num % 16;
	if(digit < 0) {
		digit *= -1;
	}
	if(digit >= 10) {
		digit += 87;
	} else {
		digit += 48;
	}
	*(current + counter) = digit;
	return 1 + counter;
}

int printHex(volatile char *current, int num) {
	if(num == 0) {
		return 0;
	}
	int counter = printHex(current, num / 16);
	int digit = num % 16;
	if(digit >= 10) {
		digit += 87;
	} else {
		digit += 48;
	}
	*(current + counter) = digit;
	return 1 + counter;
}

int printInt(volatile char *current, int num) {
	if(num == 0) {
	  	*(current) = 48;
		return 0;
	}
	if(num < 0) {
	  	*(current) = 45;
		num = num * -1;
		*(current)++;
	}
	
	int counter = printInt(current, num / 10);
	*(current + counter) = num % 10 + 48;
	return 1 + counter;
}


int printf(const char *fmt, ...) {
	va_list parameters;
	va_start(parameters, fmt);

	int num = 0;
	uint64_t pointer = 0;
  	char buffer[2000];
  	char *current = buffer;
  	while(*fmt != 0) {
		if(*fmt == '%') {
			fmt++;
			switch(*fmt) {
				case 's':;
					char *str = va_arg(parameters, char *);
					while(*str != 0) {
    					*current++ = *str++;
					}
					break;
				case 'c':;
					char ch = va_arg(parameters, int);
	    			*current++ = ch;
					break;
				case 'd':;
					num = va_arg(parameters, int);
					current += printInt(current, num);
					break;
				case 'x':;
					num = va_arg(parameters, int);
					current += printHex(current, num);
					break;
				case 'p':;
					*current++ = '0';
					*current++ = 'x';
					pointer = (uint64_t)va_arg(parameters, void *);
					current += printPointer(current, pointer);
					break;
			}
			fmt++;
		} else if(*fmt == '\n') {
			*current++ = *fmt++;
			// fmt += 1;

			// uint64_t len = 80 - ((current - buffer) % 80);
			// for (uint64_t i = 0; i< len; i++) {
			// 	*current++ = ' ';
			// }
		} else {
	    	*current++ = *fmt++;
	    	// current += 80 - ((current - buffer) % 80);
		}
  	}
  	*current = '\0';
	va_end(parameters);

	syscall_2_parameters(1, (uint64_t)&buffer);
	return 0;
}