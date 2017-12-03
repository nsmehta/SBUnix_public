#include <sys/mm.h>

void memset(void *ptr, int val, uint64_t size){
  char *p = (char *)ptr;
  while(size--){
    *p = val;
    p++;
  }
}
