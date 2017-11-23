#ifndef __PAGING_H
#define __PAGING_H
#include<sys/defs.h>

#define ALLOCATED 1
#define FREE 0

typedef struct Page{
  struct Page *nextFree;
  int status; //0 - free and 1 - allocated
  uint64_t b_addr;
}Page;

uint64_t page_alloc();
void create_free_list();

#endif
