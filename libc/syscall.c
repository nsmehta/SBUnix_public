#include <syscall.h>
#include <sys/defs.h>


// rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax
uint64_t syscall_2_parameters(uint64_t syscall_no, uint64_t p1) {

	uint64_t output = 0;

	__asm__ volatile(
		"\
		movq %2, %%rdi;\
		movq %1, %%rax;\
		int $0x80;\
		movq %%rax, %0;\
		"
		: "=r" (output)
		: "r"(syscall_no), "r"(p1)
		: "%rdi", "%rsi", "%rdx", "%rcx", "%rbp", "%rbx", "%r12", "%r13", "%r14", "%r15"
		);
	return output;

}

uint64_t syscall_3_parameters(uint64_t syscall_no, uint64_t p1, uint64_t p2) {

	uint64_t output = 0;

	__asm__ volatile(
		"\
		movq %2, %%rdi;\
		movq %3, %%rsi;\
		movq %1, %%rax;\
		int $0x80;\
		movq %%rax, %0;\
		"
		: "=r" (output)
		: "r"(syscall_no), "r"(p1), "r"(p2)
		: "%rdi", "%rsi", "%rdx", "%rcx", "%rbp", "%rbx", "%r12", "%r13", "%r14", "%r15"
		);
	return output;

}

uint64_t syscall_4_parameters(uint64_t syscall_no, uint64_t p1, uint64_t p2, uint64_t p3) {

	uint64_t output = 0;

	__asm__ volatile(
		"\
		movq %1, %%rax;\
		movq %2, %%rdi;\
		movq %3, %%rsi;\
		movq %4, %%rbp;\
		int $0x80;\
		movq %%rax, %0;\
		"
		: "=r" (output)
		: "r"(syscall_no), "r"(p1), "r"(p2), "r"(p3)
		: "%rdi", "%rsi", "%rdx", "%rcx", "%rbp", "%rbx", "%r12", "%r13", "%r14", "%r15"
		);
	return output;

}
