#include <math.h>
#include <sys/kprintf.h>

// converts octal number to decimal (base 10)
int oct2bin(unsigned char *str, int size)
{
  int n = 0;
  unsigned char *c = str;
  while (size-- > 0) {
    n *= 8;
    n += *c - '0';
    c++;
  }
  return n;
}

int octalToBinary(unsigned char *num) {
	kprintf("size string is %s", num);
	int answer = 0;
	int counter = 0;
	while(*num >= '0' && *num <= '9' && counter++ < 11) {
		answer = answer * 8 + (*num - '0');
	}
	return answer;
}

uint64_t octalStringToDecimal(char *num) {
	kprintf("----size string is %s", num);
	uint64_t answer = 0;
	int counter = 11;
	int t = 0;
	while(*num >= '0' && *num <= '9' && counter-- && t++ < 11) {
		kprintf("counter = %d, t = %d\n", counter, t);
		answer = (*num - '0') * power(8, counter - 1);
	}
	return answer;
}

uint64_t power(uint64_t base, uint64_t pow) {
	if(pow == 0)
		return 1;
	uint64_t answer = base;
	while(--pow) {
		answer *= base;
	}
	return answer;
}
