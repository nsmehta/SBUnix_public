#ifndef _FILE_H
#define _FILE_H

#include <sys/defs.h>

struct file {
  uint64_t file_count;
  uint64_t flag;
  uint64_t file_offset;
  uint64_t file_size;
  int file_error;
  void *data;
};

#define READONLY 0
#define WRITEONLY 1
#define READWRITE 2

#endif
