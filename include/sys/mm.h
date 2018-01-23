#ifndef __MM_H
#define __MM_H
#include<sys/defs.h>

/*reference: https://notes.shichao.io/lkd/ch15/*/

struct mm_struct {
  struct vm_area_struct *mmap; //list of memory areas
  //struct rb_root mm_rb; // not used currently
  struct vm_area_struct *mmap_cache; // last used memory area
  uint64_t free_area_cache; // last used memory area
  uint64_t *pdg; // page global directory
  int map_count; //number of memory areas
  uint64_t start_code; // start address of code
  uint64_t end_code; // final address of code
  uint64_t start_data; // start address of data
  uint64_t end_data; // final address of data
  uint64_t start_brk; // start address of heap
  uint64_t brk; // final address of heap
  uint64_t start_stack; // start address of stack
  uint64_t arg_start; // start of arguments
  uint64_t arg_end; // end of arguments
  uint64_t env_start; // start of environment
  uint64_t env_end; // end of environment
  uint64_t rss; // pages allocated
  uint64_t total_vm; // total no of pages
  uint64_t locked_vm; // no of locked pages
  uint64_t flags; // status flags
};

struct vm_area_struct {
  struct mm_struct *vm_mm; /* associated mm_struct */
  unsigned long vm_start; /* VMA start, inclusive */
  unsigned long vm_end; /* VMA end , exclusive */
  struct vm_area_struct *vm_next; /* list of VMA's */
  uint64_t vm_file; /* mapped file, if any */
};

void memset(void *, int, uint64_t);

#endif
