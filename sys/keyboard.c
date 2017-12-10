#include <sys/keyboard.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/isr.h>
#include <sys/io.h>

void keyboard_interrupt() {
	//kprintf("keyboard interrupt called...\n");
  uint8_t key;
  uint16_t port = 0x60;
  
  __asm__ __volatile__(
		       "inb %1, %0;"
		       :"=a" (key)
		       :"d" (port)
		       );

  port = 0x20;
  uint8_t data = 0x20;

  __asm__ __volatile__(
		       "outb %0, %1;"
		       :
		       :"a" (data), "d" (port)
		       );
  uint8_t character = hex_to_char(key);

	//kprintf("key pressed = %d\n", key);
	//kprintf("hex key pressed = %c\n", character);
  if(character != 0) {
    kprintf("%c", character);
  }
  else {
    //kprintf("wrong input! %d \n%x\n", character, key);
  }
}

uint8_t hex_to_char(uint8_t key) {

  //uint8_t character;

  if(key >= KB_1 && key <= KB_0) {
    return "1234567890"[key-KB_1];
  }

  if(key >= KB_Q && key <= KB_BRACES_CLOSE) {
    return "qwertyuiop[]"[key-KB_Q];
  }

  if(key >= KB_A && key <= KB_INVERTED_COMMA) {
    return "asdfghjkl;'"[key-KB_A];
  }

  if(key >= KB_Z && key <= KB_F_SLASH_QUESTION) {
    return "zxcvbnm,./"[key-KB_Z];
  }

  if(key >= KB_KEYPAD_7 && key <= KB_KEYPAD_DOT) {
    return "789-456+1230."[key-KB_KEYPAD_7];
  }

  if(key == KB_SPACE) {
    return ' ';
  }

  if(key == KB_ENTER) {
    kprintf("\n");
  }

  return 0;  
  
}
