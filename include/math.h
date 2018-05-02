#ifndef __MATH_H
#define __MATH_H

#include <sys/defs.h>

int oct2bin(unsigned char*, int );
int octalToBinary(unsigned char*);
uint64_t octalStringToDecimal(char*);
uint64_t power(uint64_t, uint64_t);

int printPointer(volatile char *current, uint64_t num);
int printHex(volatile char *current, int num);
int printInt(volatile char *current, int num);


#endif
