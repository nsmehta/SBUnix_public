#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/paging.h>
#include <sys/mm.h>
#include <sys/task.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
pcb *p1;
pcb *p2;

void kthread(){
  // kprintf("hello world\n");
  // switch_to(p1, p2);
  // kprintf("After switch in thread1\n");
  // switch_to(p1, p2);
  // kprintf("After switch second in thread 1\n");
  //uint64_t ep;
  //__asm__ volatile("popq %%rax; pushq %%rax":"=m" (ep) ::"%rax");
  //kprintf("ep is : %p\n", ep);
  while(1){
    kprintf("In thread one\n");
    //a++;
    switch_to(p1, p2);
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
  while(1){
    kprintf("In thread two.\n");
    switch_to(p2, p1);
  }
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  initScreen();
  pml4 *pml4_t;
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  int i = 0;
  uint64_t end = 0;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      if(i == 2)
        break;
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
  kprintf("Vlaue in pml4_t %p\n", *pml4_t);
  map_kernal(KERNBASE + (uint64_t)physbase, KERNBASE + (uint64_t)end, (uint64_t) physbase ,pml4_t);
  map_video_mem(KERNBASE + (uint64_t)0xb8000 , (uint64_t)0xb8000 , pml4_t);
  print_va_to_pa(KERNBASE + (uint64_t)0x201230 ,pml4_t);
  pml4* new_pml = (pml4 *)(KERNBASE + (uint64_t)pml4_t);
  //__asm__ volatile("mov %0, %%cr3"::"b"(pml4_t));
  //setNewVideoCardAddresses();
  kprintf("After loading cr3\n");
  //print_next_free();
  kprintf("Value pointed is: %p\n", *new_pml);
  /*p1 = (pcb*)kmalloc(sizeof(pcb));
  p2 = (pcb*)kmalloc(sizeof(pcb));
  void (*f_ptr)() = &kthread2;
  p2->kstack[127] = (uint64_t)f_ptr;
  p2->rsp = (uint64_t)(&p2->kstack[112]);

  p2->pid = 1;
  kthread();
  kprintf("the func ptr is %p\n", (uint64_t)f_ptr);
  */
  tarfsInit();
  print_vfs();
  get_file_content("/rootfs/");
  //get_file_content("/rootfs/bin/");
  //get_file_content("/rootfs/usr/");    
  kprintf("Index is %s\n", vfs[tar_get_index("/rootfs/usr/hello.c")].name);
  //kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
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
