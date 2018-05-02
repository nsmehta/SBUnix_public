#include <sys/kprintf.h>
//#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/defs.h>
//#define kernbase 0xffffffff80000000

enum dataTypes {character, integer, hex, string, pointer};
#define BUFFER 256
volatile char *videoCardEnd = (volatile char *)0xB8FA0;
volatile char *videoCardStart = (volatile char *)0xB8000;
volatile char *videoCardPosition = (volatile char *)0xB8000;



void setNewVideoCardAddresses(){
	videoCardEnd = (volatile char *)0xFFFFFFFF800B8FA0;
	videoCardStart = (volatile char *)0xFFFFFFFF800B8000;
	videoCardPosition = KERNBASE + videoCardPosition;
}

int scrollForNextCall = 0;

/*int maxof(int n_args, ...){
	register int i;
	int max, a;
	va_list ap;

	va_start(ap, n_args);
	max = va_arg(ap, int);
	for(i = 2; i <= n_args; i++) {
		if((a = va_arg(ap, int)) > max)
			max = a;
	}

	va_end(ap);
	return max;
}*/

int printPointer(volatile char *video, int colour, uint64_t num) {//long long int num) {
	if(num == 0) {
		return 0;
	}
	int counter = printPointer(video, colour, num / 16);
	int digit = num % 16;
	if(digit < 0) {
		digit *= -1;
	}
	if(digit >= 10) {
		digit += 87;
	} else {
		digit += 48;
	}
	*(video + counter) = digit;
	*(video + counter + 1) = colour;
	return 2 + counter;
}

int printHex(volatile char *video, int colour, int num) {
	if(num == 0) {
		return 0;
	}
	int counter = printHex(video, colour, num / 16);
	int digit = num % 16;
	if(digit >= 10) {
		digit += 87;
	} else {
		digit += 48;
	}
	*(video + counter) = digit;
	*(video + counter + 1) = colour;
	return 2 + counter;
}

int printInt(volatile char *video, int colour, int num) {
	if(num == 0) {
	  	*(video) = 48;
		*(video + 1) = colour;
		return 0;
	}
	if(num < 0) {
	  	*(video) = 45;
		*(video + 1) = colour;
		num = num * -1;
		*(video)++;
		*(video)++;
	}
	
	int counter = printInt(video, colour, num / 10);
	*(video + counter) = num % 10 + 48;
	*(video + counter + 1) = colour;
	return 2 + counter;
}

void kprintf(const char *fmt, ...) {
	va_list parameters;
	va_start(parameters, fmt);

	int colour = 0x00;
	int num = 0;
	uint64_t pointer = 0;
  volatile char *video = videoCardPosition;
  while(*fmt != 0) {
		if(scrollForNextCall) {
			scroll();
			video -= 160;
			videoCardPosition -= 160;
			scrollForNextCall = 0;
		}
		if(*fmt == '%') {
			fmt++;
			switch(*fmt) {
				case 's':;
					char *str = va_arg(parameters, char *);
					while(*str != 0) {
    				*video++ = *str++;
    				*video++ = colour;
					}
					break;
				case 'c':;
					char ch = va_arg(parameters, int);
    			*video++ = ch;
    			*video++ = colour;
					break;
				case 'd':;
					num = va_arg(parameters, int);
					video += printInt(video, colour, num);
					break;
				case 'x':;
					num = va_arg(parameters, int);
					video += printHex(video, colour, num);
					break;
				case 'p':;
					*video++ = '0';
					*video++ = colour;
					*video++ = 'x';
					*video++ = colour;
					pointer = (uint64_t)va_arg(parameters, void *);
					video += printPointer(video, colour, pointer);
					break;
			}
			fmt++;
		} else if(*fmt == '\n') {
			if((videoCardPosition - videoCardStart) % 160) {
				video += 160 - ((video - videoCardStart) % 160);
			} else {
				video += 160 - (video - videoCardPosition) % 160;
			}
			fmt++;
		} else {
    	*video++ = *fmt++;
    	*video++ = colour;
		}
		if(video >= videoCardEnd) {
			scrollForNextCall = 1;
		}
  }

	/*__asm__ (
	  //int funcname(int arg1, int *arg2, int arg3)
		//"int $0x80"         make the request to the OS
	  //: "=a" (res),       return result in eax ("a")
	  //  "+b" (arg1),      pass arg1 in ebx ("b")
	  //  "+c" (arg2),      pass arg2 in ecx ("c")
	  //  "+d" (arg3)       pass arg3 in edx ("d")
	  //: "a"  (128)        pass system call number in eax ("a")
	  //: "memory", "cc"
		mov	al, 0x0f		//; Cursor location low byte index
		mov	dx, 0x03D4	//; Write it to the CRT index register
		out	dx, al

	 	mov	al, bl			//; The current location is in EBX. BL contains the low byte, BH high byte
		mov	dx, 0x03D5	//; Write it to the data register
		out	dx, al			//; low byte

		);*/
		videoCardPosition = video;
		if(videoCardPosition >= videoCardEnd) {
			//videoCardPosition = videoCardStart;
			scrollForNextCall = 1;
		}

	va_end(parameters);
}

void initScreen() {
  int i;
	register char *p = (char*) videoCardStart;
	for(i = 0; i < 4000; i++) {
		*p = ' ';
		p += 2;
	}
}

void scroll() {
	int i, j;
	register char *p = (char *) videoCardStart;

	//for first 24 lines, copy contents from the next line
	for(i = 0; i < 24; i++) {
		for(j = 0; j < 160; j++) {
			*p = *(p + 160);
			p++;
		}
	}
	//for the last line, print blank spaces
	for(j = 0; j < 160; j++) {
		if(p > videoCardEnd) {
			break;
		}
		*p = ' ';
		p += 2;
	}
}
