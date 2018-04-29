
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
#include <sys/timer.h>
#include <sys/schedule.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
//extern void *user_mode_function;
pcb *g;
pcb *p2, *p1;
int count = 0;
pml4 *virtual_pml4_t;

void user_mode_function_1() {
  
  kprintf("in user function\n");

  while(1) {
    kprintf("in user mode\n");
  }
  
}


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
    set_cr3((uint64_t *)p2->cr3);
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
    set_cr3((uint64_t *)p1->cr3);
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

  // initialize timer
  timer_phase(2);
  
  
//  while(1);

  //__asm__ __volatile__ ("int $0x21");
  //__asm__ __volatile__ ("int $15");
  //__asm__ __volatile__ ("int $14");
  //while(1);
  
  initialize_free_list();
  
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
      i++;
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      end = smap->base + smap->length;
      append_free_list((uint64_t)smap->base, (uint64_t)end, (uint64_t)physfree);
    }
  }
//  while(1);
  kprintf("End is %p\n", end);
  
//  memset((void *) physfree, 0, (end - (uint64_t)physfree)/(uint64_t)8);
  
  // creates the free list: list of free and allocated physical pages
//  map_mem_to_pages(end, (uint64_t)physfree);
  
//  while(1);
  
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("physbase is : %p\n", (uint64_t)physbase);
  
  set_free_list_head();
  print_free_list();
//  while(1);
  
  pml4_t = (pml4 *)create_kernel_pml4();
//  kprintf("PAGE_US = %d \n", PAGE_US);
//  kprintf("pml4_t[511] = %p\n", pml4_t[511]);
  kprintf("kernel cr3 = %p\n", kernel_cr3);
  kprintf("*pml4_t = %p\n", pml4_t);
  
//  uint64_t *at = (uint64_t *)0x234567;
//  *at = 10;
//  kprintf("at = %p, *at = %d\n", at, *at);
  
  uint64_t free_list_page_length = get_free_list_page_length();
  
  

  kprintf("free_list_page_length = %p\n", free_list_page_length);
  kprintf("(uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (10*PAGESIZE)) = %p\n", (uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (9*PAGESIZE)));

//  while(1);

  //  map_all_kernel_pages(uint64_t v_addr_start, uint64_t v_addr_end, uint64_t p_addr_start, pml4 *kernel_pml4)
  map_all_kernel_pages((uint64_t)KERNBASE + (uint64_t)physbase, (uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (10*PAGESIZE)), (uint64_t)physbase, pml4_t);

  // mapping video memory
  map_all_kernel_pages((uint64_t)KERNBASE + (uint64_t)0xb8000 , (uint64_t)KERNBASE + (uint64_t)0xb8000 + PAGESIZE, (uint64_t)0xb8000 , pml4_t);
  
  
//  kprintf("(uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (10*PAGESIZE)) = %p\n", (uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (9*PAGESIZE)));
  
//  kprintf("total_page_length = %p\n", ((physfree - physbase) / PAGESIZE) + 10);
  
//    print_va_to_pa((uint64_t)KERNBASE + (uint64_t)physfree ,virtual_pml4_t);
//  print_va_to_pa((uint64_t)KERNBASE + (uint64_t)physbase ,pml4_t);
//  print_va_to_pa((uint64_t)KERNBASE + (uint64_t)physfree ,pml4_t);
//  //  print_va_to_pa((uint64_t)&a, pml4_t);
//  print_va_to_pa((uint64_t)KERNBASE + (uint64_t)0xb8000, pml4_t);
//  print_va_to_pa((uint64_t)0xfffffffffffff000, pml4_t);
  
//  uint64_t *a = (uint64_t *)0x2e9280;
//  kprintf("0x2e9280 = %p\n", *a);
  
//  set_new_free_list_head();

//  print_free_list_kmalloc();
//  while(1);
  
  
  // loading new value in cr3 register
  set_cr3((pml4 *)(kernel_cr3));
//  __asm__ __volatile__("movq %0, %%cr3"::"r"(kernel_cr3));
  
//  set_tss_rsp((void *) &initial_stack[INITIAL_STACK_SIZE]);
  
  
  set_new_free_list_head();
  
  setNewVideoCardAddresses();
  kprintf("new cr3 successfully set\n");

  init_gdt();
  idt_install();
  init_idt();

//  while(1);

//  uint64_t *b = (uint64_t *)(KERNBASE + 0x349000);
//  kprintf("0xffffffff80349000 = %p\n", *b);

  
  //  set top virtual address
  set_top_virtual_address((uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE) + (200*PAGESIZE));  
  
//  kprintf("(uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (1000*PAGESIZE)) = %p\n", (uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (1000*PAGESIZE)));

//  print_free_list_kmalloc();
  
//  Page *pg = NULL;
//  kprintf("null = %p\n", pg);
//  while(1);
//  uint64_t new_address = kmalloc(PAGESIZE * 23000);
//  if (new_address == 0) {
//    while(1);
//  }
//  kprintf("new_address = %p\n", new_address);

//  new_address = kmalloc(PAGESIZE * 454);
//  if (new_address == 0) {
//    while(1);
//  }
//  kprintf("new_address = %p\n", new_address);
//
//  new_address = kmalloc(PAGESIZE * 1);
//  if (new_address == 0) {
//    while(1);
//  }
//  kprintf("new_address = %p\n", new_address);
  
  kprintf("mapping done!\n");

  
//  map_kernel_pages( (uint64_t)KERNBASE + (uint64_t)physbase, (uint64_t)KERNBASE + (uint64_t)physfree + (free_list_page_length * PAGESIZE + (10*PAGESIZE)), (uint64_t)physbase, pml4_t );
  
//  set_cr3((pml4 *)kernel_cr3);
//  print_va_to_pa((uint64_t)KERNBASE + (uint64_t)physbase, (pml4 *)kernel_cr3);
//  print_va_to_pa((uint64_t)KERNBASE, (pml4 *)kernel_cr3);
  
//  p1 = (pcb*)kmalloc(sizeof(pcb));
//  p2 = (pcb*)kmalloc(sizeof(pcb));
//  kprintf("p1 = %p, %p\n", &p1, (uint64_t)&p1->kstack[127]);
//  kprintf("p2 = %p\n", &p2);
////  while(1);
//  void (*f_ptr)() = &kthread2;
//  kprintf("&kthread2 = %p\n", (uint64_t)f_ptr);
//  p2->kstack[127] = (uint64_t)f_ptr;
//  p2->rsp = (uint64_t)(&p2->kstack[127]);
//  
//  p1->cr3 = get_next_free_page_kmalloc();
//  p2->cr3 = get_next_free_page_kmalloc();
//  
//  create_new_kernel_pml4(p1->cr3, pml4_t);
//  create_new_kernel_pml4(p2->cr3, pml4_t);
  
//  kprintf("p1 = %p, cr3[511] = %p, [510] = %p\n", p1->cr3, ((uint64_t *)virtual_p1)[511], ((uint64_t *)virtual_p1)[510]);
//  kprintf("p2 = %p, cr3[511] = %p, [510] = %p\n", p2->cr3, ((uint64_t *)virtual_p2)[511], ((uint64_t *)virtual_p2)[510]);
  
//  kprintf("&p2->kstack[127] = %p\n", &p2->kstack[127]);
//  kprintf("&p2->kstack[112] = %p\n", &p2->kstack[112]);

//  p2->pid = 1;
//  kthread();
  
//  set_cr3((pml4 *)kernel_cr3);
  
//  pcb *up = (pcb *)kmalloc(sizeof(pcb));
//  kprintf("up = %p, kstack[127] = %p,\n[126] = %p\n", &up, (uint64_t)&up->kstack[127], (uint64_t)&up->kstack[126]);
//  
//  up->cr3 = get_next_free_page_kmalloc();
//  
//  set_cr3((pml4 *)up->cr3);
//  
//  create_new_user_pml4(up->cr3, pml4_t);
//  
//  uint64_t *user_stack = (uint64_t *) kmalloc(PAGESIZE);
//  
//  
//  up->kstack[127] = 0x23; // SS
//  up->kstack[126] = (uint64_t) &user_stack[511]; // RSP
//  up->kstack[125] = (uint64_t)0x203; // FLAGS
//  up->kstack[124] = 0x2b; // CS
//  up->kstack[123] = (uint64_t) &user_mode_function; // RIP
//  
//  // setting the TSS RSP to the kernel stack
//  set_tss_rsp((void *) &(up->kstack[127]));
//  
//  up->rsp = (uint64_t) &up->kstack[127];
  
//  kprintf("up->rsp = %p, kstack[127] = %p\n", up->rsp, up->kstack[127]);
  
//  switch_to_user_mode(up);
  
  
//  char *filename = "bin/sbush";
//  Elf64_Ehdr *p = get_elf(filename);
//  kprintf("\nreturned to main %x%c\n", p->e_ident[0], p->e_ident[1]);
//  int result = validate_elf_header(p);
//  kprintf("result = %d\n", result);
//  result = check_elf_loadable(p);
//  kprintf("elf loadable = %d\n", result);
//  struct mm_struct *head = NULL;
//  if(result) {
//    head = load_elf_vmas(p);
//  }
//  kprintf("mm_struct = %p\n", head->mmap->vm_start);
//  int count = 0;
//  struct vm_area_struct *temp = head->mmap;
//  while(temp != NULL){
//    kprintf("vm_start = %d\n", temp->vm_type);
//    temp = temp->vm_next;
//    count++;
//  }
//  kprintf("count = %d\n", count);
//
//
//  struct pcb *first_process = create_process(head);
//  kprintf("first process = %p\n", (uint64_t)first_process);
  
/*  uint64_t flags = 0x0;
  
  __asm__ volatile(
  "\
    pushfq;\
    popq  %0;\
  "
  :"=r" (flags)
  :
  );
  
  first_process_switch(first_process, (uint64_t) &user_mode_function_1, 0x200202);
  __asm__ __volatile__ ("sti");
*/
  
//  for(uint64_t i = 0; i < 100000; i++) {
//    for(uint64_t j = 0; j < 10000; j++) {
//      
//    }
//  }
  
//  user_mode_function_1();
  
  struct pcb *dummy_process = (struct pcb *)create_dummy_process();
//  
  scheduler_init();
  schedule_idle_process(dummy_process, (uint64_t) &user_mode_function_1);
  
  

  kprintf("in main\n");
  __asm__ __volatile__ ("sti");
  

  while(1);

  
  /*
  ************************************************************************************************************
  */
  
  memset((void *) pml4_t, 0, (uint64_t)PAGESIZE/(uint64_t)8);
//  int a = 0;
  kprintf("End is %p\n", end);
  kprintf("Value in pml4_t %p\n", *pml4_t);
  kprintf("Value of pml4_t %p\n", (uint64_t)pml4_t);
  virtual_pml4_t = (uint64_t *)((uint64_t)pml4_t + (uint64_t)KERNBASE);
  kprintf("kernbase = %p\n", (uint64_t)KERNBASE);
  kprintf("Value in virtual_pml4_t %p\n", virtual_pml4_t);
  virtual_pml4_t[511] = ((uint64_t)pml4_t | 0x7);
  kprintf("value at [511] = %p\n", virtual_pml4_t[511]);


  // identity mapping the pages between physbase and end in a 1:1 mapping starting from KERNBASE
  map_kernel((uint64_t)KERNBASE + (uint64_t)physbase, (uint64_t)KERNBASE + (uint64_t)end, (uint64_t) physbase ,virtual_pml4_t);
  // mappinng the video memory to point to changed addresses
  map_video_mem((uint64_t)KERNBASE + (uint64_t)0xb8000 , (uint64_t)0xb8000 , virtual_pml4_t);

  //map_kernel((uint64_t)&_binary_tarfs_start, (uint64_t)&_binary_tarfs_end, (uint64_t)&_binary_tarfs_start - KERNBASE,pml4_t);
  //map_kernel((uint64_t)&a, PAGESIZE + (uint64_t)&a, (uint64_t)&a - KERNBASE ,pml4_t);
  
//  print_va_to_pa((uint64_t)KERNBASE + (uint64_t)0x349000 ,virtual_pml4_t);
//  print_va_to_pa((uint64_t)&a, virtual_pml4_t);
//  print_va_to_pa((uint64_t)0xfffffffffffff000, virtual_pml4_t);
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

  
//  p1 = (pcb*)kmalloc(sizeof(pcb));
//  p2 = (pcb*)kmalloc(sizeof(pcb));
//  kprintf("p1 = %p\n", &p1);
//  kprintf("p2 = %p\n", &p2);
//  while(1);
//  void (*f_ptr)() = &kthread2;
//  p2->kstack[127] = (uint64_t)f_ptr;
//  p2->rsp = (uint64_t)(&p2->kstack[112]);
//
//  p2->pid = 1;
//  kthread();


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
  

  //for thr first user process:
  /*
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
  //ring_3_switch((uint64_t)q);
  kprintf("after switch to ring 3\n");

  kprintf("End is %p\n", end);
  */
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
