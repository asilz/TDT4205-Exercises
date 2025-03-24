.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.string0: .asciz "a = "
.string1: .asciz "a < 8:  "
.string2: .asciz "a < 5:  "
.string3: .asciz "a <= 5: "
.string4: .asciz "a == 5: "
.string5: .asciz "a != 5: "
.string6: .asciz "!(a == 5): "
.string7: .asciz "a >= 5: "
.string8: .asciz "a > 5:  "
.string9: .asciz "a > -8: "
.section .bss
.align 8
.text
.start:
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
	movq $8, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	setl %al
	movzbq %al, %rax
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
	cmpq %rcx, %rax
	setl %al
	movzbq %al, %rax
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
	cmpq %rcx, %rax
	setle %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string4(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string5(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	setne %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string6(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	sete %al
	movzbq %al, %rax
	not %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string7(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	setge %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string8(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $5, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	setg %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	leaq .string9(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $-8, %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	cmpq %rcx, %rax
	setg %al
	movzbq %al, %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $0, %rax
	jmp .start.epilogue
.start.epilogue:
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
	call .start
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
