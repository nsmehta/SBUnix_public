#include <sys/elf64.h>
#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <string.h>
#include <sys/defs.h>

int validate_elf_header(Elf64_Ehdr *ehdr) {
  //Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)ehdr + sizeof(struct posix_header_ustar));
  kprintf("e_ident = %s\n", ehdr->e_ident);
  //kprintf("e_ident = %s\n", header->e_ident);
  if(ehdr->e_ident[0] == 0x7f && strsubcmp("ELF", (char *)ehdr->e_ident, 1, 4) == 0) {
    kprintf("phnum = %d ", ehdr->e_phnum);
    return 1;
  }
  return 0;
}

int validate_elf_header_from_fp(struct file *fp) {
  Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)fp->data + sizeof(struct posix_header_ustar));
  kprintf("e_ident = %s\n", header->e_ident);
  //kprintf("e_ident = %s\n", header->e_ident);
  if(header->e_ident[0] == 0x7f && strsubcmp("ELF", (char *)header->e_ident, 1, 4) == 0) {
    kprintf("phnum = %d ", header->e_phnum);
    return 1;
  }
  return 0;
}

int check_elf_loadable(Elf64_Ehdr *ehdr) {
  //Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)ehdr + sizeof(struct posix_header_ustar));
    int i = 0;
    for(i = 0; i< ehdr->e_phnum; i++) {
      Elf64_Phdr *phdr = ((Elf64_Phdr *)(((uint64_t)ehdr) + ehdr->e_phoff)) + i;
      if(phdr->p_type == PT_LOAD) {
	return 1;
      }
    }
    return 0;
}

int check_elf_loadable_from_fp(struct file *fp) {
  Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)fp->data + sizeof(struct posix_header_ustar));
    int i = 0;
    for(i = 0; i< header->e_phnum; i++) {
      Elf64_Phdr *phdr = ((Elf64_Phdr *)(((uint64_t)header) + header->e_phoff)) + i;
      if(phdr->p_type == PT_LOAD) {
	return 1;
      }
    }
    return 0;
}
