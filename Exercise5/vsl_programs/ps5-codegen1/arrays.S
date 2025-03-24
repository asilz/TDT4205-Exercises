.section .rodata
intout: .asciz "%ld"
strout: .asciz "%s"
errout: .asciz "Wrong number of arguments"
.string0: .asciz "The values of the array are: "
.string1: .asciz " "
.string2: .asciz " "
.string3: .asciz " "
.string4: .asciz "array[3] = "
.section .bss
.align 8
scalar: .zero 8
array: .zero 80
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
	movq $0, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	pushq %rcx
	movq $1, %rax
	popq %rcx
	movq %rax, (%rcx)
	movq $1, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	pushq %rcx
	movq $1, %rax
	pushq %rax
	movq $0, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	popq %rcx
	addq %rcx, %rax
	popq %rcx
	movq %rax, (%rcx)
	movq $1, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	pushq %rcx
	movq $3, %rax
	popq %rcx
	movq %rax, (%rcx)
	leaq .string0(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $0, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq .string1(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $1, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq .string2(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $2, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	leaq .string3(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $3, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	movq %rax, %rsi
	leaq intout(%rip), %rdi
	call safe_printf
	movq $'\n', %rdi
	call putchar
	movq $3, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	pushq %rcx
	movq $1, %rax
	popq %rcx
	movq %rax, (%rcx)
	movq $1, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	pushq %rax
	movq $0, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	pushq %rax
	popq %rdi
	popq %rsi
	call .sum
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	pushq %rcx
	movq $2, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	pushq %rax
	movq $2, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
	pushq %rax
	popq %rdi
	popq %rsi
	call .sum
	popq %rcx
	movq %rax, (%rcx)
	leaq .string4(%rip), %rsi
	leaq strout(%rip), %rdi
	call safe_printf
	movq $3, %rax
	leaq array(%rip), %rcx
	leaq (%rcx, %rax, 8), %rcx
	movq (%rcx), %rax
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
.sum:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	movq -16(%rbp), %rax
	pushq %rax
	movq -8(%rbp), %rax
	popq %rcx
	addq %rcx, %rax
	jmp .sum.epilogue
.sum.epilogue:
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
