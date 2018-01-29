
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/paging.h>
#include <sys/mm.h>
#include <sys/task.h>
#include <sys/elf64.h>
#include <sys/file.h>
#include <sys/keyboard.h>
#include <sys/idt.h>
#include <sys/isr.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
pcb *p1;
pcb *p2;
int count = 0;

void kthread(){
  // kprintf("hello world\n");
  // switch_to(p1, p2);
  // kprintf("After switch in thread1\n");
  // switch_to(p1, p2);
  // kprintf("After switch second in thread 1\n");
  //uint64_t ep;
  //__asm__ volatile("popq %%rax; pushq %%rax":"=m" (ep) ::"%rax");
  //kprintf("ep is : %p\n", ep);
  //int count = 0;

  kprintf("In thread one\n");
  while(1){
    kprintf("In thread one\n");
    //a++;
    count += 1;
    switch_to(p1, p2);
    count += 1;
    if(count >= 2) {
      break;
    }
  }
}

void kthread2(){
  // kprintf("in second thread\n");
  // switch_to(p2, p1);
  // kprintf("PID in p2 is %p\n", p2->pid);
  // kprintf("value in rsp for p1 is %p\n", p1->rsp);
  // switch_to(p2, p1);
  // //kprintf("Hello hello hello %p\n", p1->rsp);
  // while(1);
  //int count = 0;
  while(1){
    kprintf("In thread two.\n");
    switch_to(p2, p1);
    count += 1;
    //if(count >= 2) {
    // break;
    //}
  }
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  initScreen();


  //__asm__ __volatile__ ("int $0x21");
  //__asm__ __volatile__ ("int $15");
  //__asm__ __volatile__ ("int $14");
  //while(1);
  pml4 *pml4_t;
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  int i = 0;
  uint64_t end = 0;
  kprintf("modulep[0]=%p\n", modulep[0]);
  kprintf("modulep[1]=%p\n", modulep[1]);
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      //if(i == 2)
      //  break;
      i++;
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      end = smap->base + smap->length;
    }
  }
  kprintf("End is %p\n", end);
  map_mem_to_pages(end, (uint64_t)physfree);
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("physbase is : %p\n", (uint64_t)physbase);
  pml4_t = (pml4 *)get_next_free_page();
  memset((void *) pml4_t, 0, PAGESIZE);
  int a = 0;
  kprintf("End is %p\n", end);
  kprintf("Value in pml4_t %p\n", *pml4_t);
  //kprintf("pml4_t = %p\n", pml4_t);
  //while(1);
  // identity mapping the pages between physbase and end in a 1:1 mapping starting from KERNBASE
  map_kernel(KERNBASE + (uint64_t)physbase, KERNBASE + (uint64_t)end, (uint64_t) physbase ,pml4_t);
  // mappinng the video memory to point to changed addresses
  map_video_mem(KERNBASE + (uint64_t)0xb8000 , (uint64_t)0xb8000 , pml4_t);

  //map_kernel((uint64_t)&_binary_tarfs_start, (uint64_t)&_binary_tarfs_end, (uint64_t)&_binary_tarfs_start - KERNBASE,pml4_t);
  //map_kernel((uint64_t)&a, PAGESIZE + (uint64_t)&a, (uint64_t)&a - KERNBASE ,pml4_t);
  
  print_va_to_pa(KERNBASE + (uint64_t)0x201230 ,pml4_t);
  print_va_to_pa((uint64_t)&a, pml4_t);
  //print_va_to_pa((uint64_t)&_binary_tarfs_start, pml4_t);
  //pml4* new_pml = (pml4 *)(KERNBASE + (uint64_t)pml4_t);

  //kprintf("bin start, end: %p, %p, %p\n", (uint64_t)&_binary_tarfs_start, (uint64_t)&_binary_tarfs_end, &a);

  //struct posix_header_ustar *p = (struct posix_header_ustar *)(&_binary_tarfs_start);
  //kprintf("p->size = %x\n", p->size);

  //kprintf("pml4_t = %p\n", pml4_t);
  
  set_cr3(pml4_t);
  setNewVideoCardAddresses();
  //setNewTarfsAddress();
  kprintf("After loading cr3\n");

  init_gdt();
  idt_install();
  init_idt();

  
  p1 = (pcb*)kmalloc(sizeof(pcb));
  p2 = (pcb*)kmalloc(sizeof(pcb));
  void (*f_ptr)() = &kthread2;
  p2->kstack[127] = (uint64_t)f_ptr;
  p2->rsp = (uint64_t)(&p2->kstack[112]);

  p2->pid = 1;
  kthread();


  // for triggering page fault:
  //struct file *file_pointer = (struct file *)(sizeof(struct file));
  //file_pointer->data = (uint64_t *)0xffffffff80808080;
  //kprintf("value = %p\n", file_pointer->data);

  tarfs_init();
  //print_vfs();

  //get_file_content("/rootfs/");
  //get_file_content("usr/hello.c");
  //get_file_content("/rootfs/bin/");
  //get_file_content("/rootfs/usr/");
  //get_file_content("/rootfs/bin/sbush");
  //kprintf("returned to main\n");
  //get_file_content("/rootfs/lib/libc.a");
  //kprintf("returned to main\n");
  
  
  char *filename = "bin/sbush";
  Elf64_Ehdr *p = get_elf(filename);
  kprintf("\nreturned to main %x%c\n", p->e_ident[0], p->e_ident[1]);
  int result = validate_elf_header(p);
  kprintf("result = %d\n", result);
  result = check_elf_loadable(p);
  kprintf("elf loadable = %d\n", result);
  struct mm_struct *head = NULL;
  if(result) {
    head = load_elf_vmas(p);
  }
  kprintf("mm_struct = %p\n", head->mmap->vm_start);
  int count = 0;
  struct vm_area_struct *temp = head->mmap;
  while(temp != NULL){
    kprintf("vm_start = %d\n", temp->vm_type);
    temp = temp->vm_next;
    count++;
  }
  kprintf("count = %d\n", count);
  void *q = (void *)p->e_entry;
  kprintf("value at start address = %p\n", q);
  ring_3_switch((uint64_t)q);
  kprintf("after switch to ring 3\n");


  /*
  Elf64_Ehdr *q = get_elf("lib/crt1.o");
  kprintf("\nreturned to main %x%c\n", q->e_ident[0], q->e_ident[1]);
  result = validate_elf_header(q);
  kprintf("result = %d\n", result);
  result = check_elf_loadable(q);
  kprintf("elf loadable = %d\n", result);
  
  struct posix_header_ustar *r = get_tarfs("bin/hello.c");
  struct file *fp = open_tarfs(r);
  kprintf("\nfp = %p\n", fp);
  result = validate_elf_header_from_fp(fp);
  kprintf("validate_elf_header_from_fp = %d\n", result);
  result = check_elf_loadable_from_fp(fp);
  kprintf("elf loadable = %d\n", result);
  */



  while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );

  init_gdt();
  idt_install();
  init_idt();


  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
