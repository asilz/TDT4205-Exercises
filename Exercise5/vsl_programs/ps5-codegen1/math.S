.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.string0: .asciz "num = "
.string1: .asciz "num*3 = "
.string2: .asciz "num+5 = "
.string3: .asciz "num/5 = "
.string4: .asciz "4*-num = "
.string5: .asciz "(num-1)*(num+1)*(num+3) = "
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
	leaq .string0(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string1(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $3, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	imulq %rcx, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string2(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	addq %rcx, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string3(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cqo
	idivq %rcx
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string4(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq -8(%rbp), %rax
	negq %rax
	pushq %rax
	movq $4, %rax
	popq %rcx
	imulq %rcx, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string5(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $3, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	addq %rcx, %rax
	pushq %rax
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	addq %rcx, %rax
	pushq %rax
	movq $1, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	subq %rcx, %rax
	popq %rcx
	imulq %rcx, %rax
	popq %rcx
	imulq %rcx, %rax
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
