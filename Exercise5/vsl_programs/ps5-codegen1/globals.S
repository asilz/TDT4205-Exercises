.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.string0: .asciz "On startup, A and B are "
.string1: .asciz " "
.string2: .asciz "Now, B is "
.string3: .asciz "Here, A is "
.section .bss
.align 8
A: .zero 8
B: .zero 8
dummy: .zero 8
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
	leaq .string0(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	leaq A(%rip), %rax
	movq (%rax), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq .string1(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	leaq B(%rip), %rax
	movq (%rax), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $5, %rax
	leaq A(%rip), %rcx
	movq %rax, (%rcx)
	call .otherFunc
	leaq .string2(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	leaq B(%rip), %rax
	movq (%rax), %rax
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
.otherFunc:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	leaq .string3(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	leaq A(%rip), %rax
	movq (%rax), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $2, %rax
	leaq B(%rip), %rcx
	movq %rax, (%rcx)
	movq $0, %rax
	jmp .otherFunc.epilogue
.otherFunc.epilogue:
	movq %rbp, %rsp
	popq %rbp
	ret
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq $0, %rdi
	jne ABORT
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
