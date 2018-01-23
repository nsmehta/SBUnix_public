#ifndef _TARFS_H
#define _TARFS_H

#include <sys/defs.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>
#include <sys/elf64.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

void tarfs_init();

// tarfs data structure:

typedef struct tarfs {
  char name[128];
  int parent;
  uint64_t size;
  int type;
  uint64_t address;
  int index;
  int offset;
} tarfs;

tarfs vfs[2048];
int vfs_pointer;
void print_vfs();
uint64_t tar_lookup(char *);
uint64_t tar_ls(char *);
int tar_get_index(char *);
void get_file_content(char *);
int test_offset(char *);
int get_file_offset(char *);
//Elf64_Ehdr *get_elf(char *);
struct posix_header_ustar *get_tarfs(char *);
struct posix_header_ustar *traverse_tarfs(char *);
struct file *open_tarfs(struct posix_header_ustar *);
struct file *open_tarfs_from_elf(Elf64_Ehdr *);
void setNewTarfsAddress();
#endif
