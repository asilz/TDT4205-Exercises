.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.section .bss
.align 8
.text
.main:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	call .addOne
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $0, %rax
	jmp .main.epilogue
.main.epilogue:
	movq %rbp, %rsp
	popq %rbp
	ret
.addOne:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	addq %rcx, %rax
	jmp .addOne.epilogue
.addOne.epilogue:
	movq %rbp, %rsp
	popq %rbp
	ret
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq $1, %rdi
	jne ABORT
	addq $8, %rsi
	movq %rdi, %rcx
PARSE_ARGV:
	pushq %rsi
	pushq %rcx
	movq (%rsi), %rdi
	movq $0, %rsi
	movq $10, %rdx
	call strtol
	popq %rcx
	popq %rsi
	pushq %rax
	subq $8, %rsi
	loop PARSE_ARGV
	popq %rdi
	call .main
	movq %rax, %rdi
	call exit
ABORT:
	leaq errout(%rip), %rdi
	call puts
	movq $1, %rdi
	call exit
safe_printf:
	pushq %rbp
	movq %rsp, %rbp
	andq $-16, %rsp
	call printf
	movq %rbp, %rsp
	popq %rbp
	ret
.global main
