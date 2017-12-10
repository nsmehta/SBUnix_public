.text

# IRQs

.global irq32
.global irq33
.global irq34
.global irq35
.global irq36
.global irq37
.global irq38
.global irq39
.global irq40
.global irq41
.global irq42
.global irq43
.global irq44
.global irq45
.global irq46
.global irq47

# 32: Stack Fault Exception
irq32:
  cli
  pushq $0 		# dummy error code
  pushq $32 		# irq number
  jmp irq_common_stub

# 33: General Protection Fault Exception
irq33:
  cli
  pushq $0 		# dummy error code
  pushq $33 		# irq number
  jmp irq_common_stub

# 34: Page Fault Exception
irq34:
  cli
  pushq $0 		# dummy error code
  pushq $34 		# irq number
  jmp irq_common_stub

# 35: Unknown Interrupt Exception
irq35:
  cli
  pushq $0 		# dummy error code
  pushq $35 		# irq number
  jmp irq_common_stub

# 36: Coprocessor Fault Exception
irq36:
  cli
  pushq $0 		# dummy error code
  pushq $36 		# irq number
  jmp irq_common_stub

# 37: Alignment Check Exception (486+)
irq37:
  cli
  pushq $0 		# dummy error code
  pushq $37 		# irq number
  jmp irq_common_stub

# 38: Machine Check Exception (Pentium/586+)
irq38:
  cli
  pushq $0 		# dummy error code
  pushq $38 		# irq number
  jmp irq_common_stub

# 39: Reserved Exceptions
irq39:
  cli
  pushq $0 		# dummy error code
  pushq $39 		# irq number
  jmp irq_common_stub

# 40: Reserved Exceptions
irq40:
  cli
  pushq $0 		# dummy error code
  pushq $40 		# irq number
  jmp irq_common_stub

# 41: Reserved Exceptions
irq41:
  cli
  pushq $0 		# dummy error code
  pushq $41 		# irq number
  jmp irq_common_stub

# 42: Reserved Exceptions
irq42:
  cli
  pushq $0 		# dummy error code
  pushq $42 		# irq number
  jmp irq_common_stub

# 43: Reserved Exceptions
irq43:
  cli
  pushq $0 		# dummy error code
  pushq $43 		# irq number
  jmp irq_common_stub

# 44: Reserved Exceptions
irq44:
  cli
  pushq $0 		# dummy error code
  pushq $44 		# irq number
  jmp irq_common_stub

# 45: Reserved Exceptions
irq45:
  cli
  pushq $0 		# dummy error code
  pushq $45 		# irq number
  jmp irq_common_stub

# 46: Reserved Exceptions
irq46:
  cli
  pushq $0 		# dummy error code
  pushq $46 		# irq number
  jmp irq_common_stub

# 47: Reserved Exceptions
irq47:
  cli
  pushq $0 		# dummy error code
  pushq $47 		# irq number
  jmp irq_common_stub


.extern irq_handler
# IRQ common stub. It saves processor state, sets up for kernel mode segments
# calls the C-level fault handler, and finally restores the stack frame.

irq_common_stub:
  # pusha
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rbx
  pushq %rsp
  pushq %rbp
  pushq %rsi
  pushq %rdi

  movq %ds, %rax
  pushq %rax

  movq $0x10, %rax
  movq %rax, %ds
  movq %rax, %es
  movq %rax, %fs
  movq %rax, %gs

  callq irq_handler

  popq %rbx
  movq %rbx, %ds
  movq %rbx, %es
  movq %rbx, %fs
  movq %rbx, %gs

  # popa
  popq %rdi
  popq %rsi
  popq %rbp
  popq %rsp
  popq %rbx
  popq %rdx
  popq %rcx
  popq %rax


  add $0x08, %rsp
  sti
  iretq
