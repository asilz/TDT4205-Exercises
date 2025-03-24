.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.string0: .asciz "Printing the sum of all integers from "
.string1: .asciz " up to, but not including "
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
	pushq $0
	leaq .string0(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq .string1(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq -16(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq -16(%rbp), %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	popq %rsi
	call .sumRange
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
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
.sumRange:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	pushq $0
	pushq $0
	movq -16(%rbp), %rax
	pushq %rax
	popq %rdi
	call .sumUntil
	movq %rax, -24(%rbp)
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	call .sumUntil
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	pushq %rax
	movq -24(%rbp), %rax
	popq %rcx
	subq %rcx, %rax
	jmp .sumRange.epilogue
.sumRange.epilogue:
	movq %rbp, %rsp
	popq %rbp
	ret
.sumUntil:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	pushq $0
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	subq %rcx, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	imulq %rcx, %rax
	movq %rax, -16(%rbp)
	movq $2, %rax
	pushq %rax
	movq -16(%rbp), %rax
	popq %rcx
	cqo
	idivq %rcx
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	jmp .sumUntil.epilogue
.sumUntil.epilogue:
	movq %rbp, %rsp
	popq %rbp
	ret
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $1, %rdi
	cmpq $2, %rdi
	jne ABORT
	addq $16, %rsi
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
	popq %rsi
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
