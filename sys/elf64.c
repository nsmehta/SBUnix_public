#include <sys/elf64.h>
#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <string.h>
#include <sys/defs.h>
#include <sys/mm.h>
#include <sys/paging.h>
#include <sys/schedule.h>

// check if the ELF magic number exists at the start of the header
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


// check if ELF contains Program Headers of type PT_LOAD
int check_elf_loadable(Elf64_Ehdr *ehdr) {
  //Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)ehdr + sizeof(struct posix_header_ustar));
    int i = 0;
    for(i = 0; i < ehdr->e_phnum; i++) {
      Elf64_Phdr *phdr = ((Elf64_Phdr *)(((uint64_t)ehdr) + ehdr->e_phoff)) + i;
      if(phdr->p_type == PT_LOAD) {
	kprintf("p->p_vaddr = %p\n",phdr->p_vaddr);
	kprintf("p->p_paddr = %p\n", phdr->p_paddr);
	kprintf("p->p_memsz = %p\n",phdr->p_memsz);
	kprintf("p->p_filesz = %p\n",phdr->p_filesz);
	return 1;
      }
    }
    return 0;
}

int check_elf_loadable_from_fp(struct file *fp) {
  Elf64_Ehdr *header = (Elf64_Ehdr *)((char *)fp->data + sizeof(struct posix_header_ustar));
    int i = 0;
    for(i = 0; i < header->e_phnum; i++) {
      Elf64_Phdr *phdr = ((Elf64_Phdr *)(((uint64_t)header) + header->e_phoff)) + i;
      if(phdr->p_type == PT_LOAD) {
	return 1;
      }
    }
    return 0;
}


// returns the mm_struct for an ELF file pointer
struct mm_struct *load_elf_vmas(Elf64_Ehdr *ehdr) {
  struct mm_struct *mm_head = (struct mm_struct *)kmalloc_user(sizeof(struct mm_struct));
  int i = 0;
  struct vm_area_struct *vma_head = NULL;
  struct vm_area_struct *vma_next = NULL;
  kprintf("starting address = %p\n", ehdr->e_entry);

  for(i = 0; i < ehdr->e_phnum; i++) {
    Elf64_Phdr *phdr = ((Elf64_Phdr *)(((uint64_t)ehdr) + ehdr->e_phoff)) + i;
    if(phdr->p_type == PT_LOAD) {
      if(vma_head == NULL) {
	vma_head = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
	vma_head->vm_start = phdr->p_vaddr;
	vma_head->vm_end = (phdr->p_vaddr + phdr->p_memsz);
	vma_head->vm_type = TYPE_FILE;
  vma_head->vm_offset = phdr->p_offset;
	//vma_head->vm_next = vma_next;
	vma_head->vm_next = NULL;
	vma_next = vma_head;
	kprintf("p_offset = %p\n", phdr->p_offset);
      }
      else {
	vma_next->vm_next = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
	vma_next = vma_next->vm_next;
	vma_next->vm_start = phdr->p_vaddr;
	vma_next->vm_end = (phdr->p_vaddr + phdr->p_memsz);
	vma_next->vm_type = TYPE_FILE;
  vma_next->vm_offset = phdr->p_offset;
	vma_next->vm_next = NULL;
	//vma_next = vma_next->vm_next;
      }
    }
    
  }

  // assign stack and heap
  if(vma_head == NULL) {
    vma_head = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
    vma_head->vm_start = (uint64_t)NULL;
    vma_head->vm_end = (uint64_t)NULL;
    vma_head->vm_type = TYPE_STACK;
    //vma_head->vm_next = vma_next;
    vma_head->vm_next = NULL;
    vma_next = vma_head;
    
    vma_next->vm_next = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
    vma_next = vma_next->vm_next;
    vma_next->vm_start = (uint64_t)NULL;
    vma_next->vm_end = (uint64_t)NULL;
    vma_next->vm_type = TYPE_HEAP;
    vma_next->vm_next = NULL;
    //vma_next = vma_next->vm_next;
    
  }
  else {
    vma_next->vm_next = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
    vma_next = vma_next->vm_next;
    kprintf("vma_head = %p, vma_head->vm_next = %p, vma_next = %p\n", vma_head, vma_head->vm_next, vma_next);
    vma_next->vm_start = (uint64_t)NULL;
    vma_next->vm_end = (uint64_t)NULL;
    vma_next->vm_type = TYPE_STACK;
    vma_next->vm_next = NULL;
    //vma_next = vma_next->vm_next;

    vma_next->vm_next = (struct vm_area_struct *)kmalloc_user(sizeof(struct vm_area_struct));
    vma_next = vma_next->vm_next;
    vma_next->vm_start = (uint64_t)NULL;
    vma_next->vm_end = (uint64_t)NULL;
    vma_next->vm_type = TYPE_HEAP;
    vma_next->vm_next = NULL;
    //vma_next = vma_next->vm_next;    
  }
  
  mm_head->e_entry = ehdr->e_entry;
  mm_head->elf_head = (uint64_t *)ehdr;
  mm_head->mmap = vma_head;
  //mm_head->start_stack = kmalloc(PAGESIZE * 2);
  //mm_head->start_brk = kmalloc(PAGESIZE * 2);

  return mm_head;  
}
