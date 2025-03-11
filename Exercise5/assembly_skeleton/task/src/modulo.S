.globl main

.data

n_rows: .quad 12
n_cols: .quad 12
space_for_numbers: .quad 2

space: .string " "
newline: .string "\n"

integer_slot: .string "%ld"

.text
/*
    Main "function" - program starts here!

    It works as a regular function, but is called at when running the program instead of by other code.

    We allow main to use the callee-save/"global" registers:
    %rbx, %rbp, and %r12-r15

    Our program should probably use loops to iterate through the table.
    Thus, we will probably use variables/counters tracking the current loop.
    It might also be a good idea to use another variable to store the modulo result.

    How you handle this is up to you,
    but you may assign certain registers that *always* represent certain variables.
    E.g.:
        r12 = i
        r13 = j
        r14 = modulo_result
        etc...

    Note that we do have a finite amount of "global" registers.
    We use these as we *know* they cannot (or at least *should not*) be overwritten by other functions.

    We recommend that you keep a AT&T Assembly cheat sheet like this nearby:
    https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf
*/
main:
    pushq %rbp
    movq %rsp, %rbp

    movq $1, %r12

    outer_loop_start:
        movq $1, %r13

        leaq n_rows(%rip), %rcx
        movq (%rcx), %rcx
        cmpq %r12, %rcx
        jl outer_loop_end


        inner_loop_start:
            leaq n_rows(%rip), %rcx
            movq (%rcx), %rcx
            cmpq %r13, %rcx
            jl inner_loop_end

            movq $0, %rdx
            movq %r12, %rax

            divq %r13
            movq %rdx, %rbx

            movq %rbx, %rdi
            call get_digits

            movq $3, %rsi
            subq %rax, %rsi

            movq %rbx, %rdi
            call print_padded_number

            # We are now done with a single number

            inc %r13
            jmp inner_loop_start

        inner_loop_end:
        # INNER LOOP END

            movq $'\n', %rdi
            call putchar
            inc %r12
            jmp outer_loop_start

    outer_loop_end:
    # OUTER LOOP END
        # After completing the outer loop we should be done with the table
    
    movq $0, %rax
    leave
    ret


/*
    Functions are defined here.
    These will be executed when we call them using the `call` keyword.
*/

/*
    Function: print_padded_number

    Prepends PADDING spaces before NUMBER, and prints it to output

    Args:
        (%rdi) NUMBER (quad)  - Number to print
        (%rsi) PADDING (quad) - Number of padding spaces before number 
*/
print_padded_number:
    pushq %rbp
    movq %rsp, %rbp

    pushq %rdi
    test %rsi, %rsi # check if number of spaces is 0
    je print_loop_end

    print_loop_start:
    # Print Space Loop START
    
        pushq %rsi
        call print_space
        popq %rsi

        dec %rsi
        jne print_loop_start

    print_loop_end:

    popq %rsi
    leaq integer_slot(%rip), %rdi
    call safe_printf

    # Done
    leave
    ret

/*
    Function: print_space

    Simply prints a space.
*/
print_space:
    pushq %rbp
    movq %rsp, %rbp

    leaq space(%rip), %rdi
    call safe_printf

    leave
    ret

/*
    Function: get_digits

    Calculates the digits of NUMBER, and returns it as N_DIGITS

    Args:
        (%rdi) NUMBER (quad)  - Number to print

    Returns:
        (%rax) N_DIGITS (quad) - Number of digits in NUMBER 
*/
get_digits:
    pushq %rbp
    movq %rsp, %rbp

    movq $1, %r11           # r11 = counter = 1

    # Ready argument for division
    movq %rdi, %rax
    
gd_loop:
    # Divide NUMBER by 10
    movq $10, %r8

    # Ensure that %rdx = 0; divq will use it as upper bits...
    movq $0, %rdx
    # %rax = %rax / 10
    divq %r8

    # If quotient == 0, exit the loop
    movq $0, %r8

    cmp %rax, %r8
    je gd_loop_end

    # Else, increment digit counter, and repeat
    # Note, result from division is already in the correct register
    inc %r11
    jmp gd_loop

gd_loop_end:
    # Our counter contains number of digits; move it to return register %rax
    movq %r11, %rax

    leave
    ret


/*
    Function: safe_printf

    Prints to output - like printf - but makes sure that stack is alligned before calling printf
    Convinience function that avoids a lot of crashes caused by printf

    Args:
        (all) any value, but always a string pointer in (%rdi)
*/
safe_printf:
	pushq %rbp
	movq %rsp, %rbp

	andq $-16, %rsp
	call printf
    
	leave
	ret

// Workaround for macOS prefixing symbols with _
#ifdef __APPLE__
    .set printf, _printf
    .set putchar, _putchar
    .set _main, main
    .global _main
#endif
