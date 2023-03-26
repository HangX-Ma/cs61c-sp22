.globl factorial

.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:
    # YOUR CODE HERE
    addi sp, sp, -8     # adjust stack for 2 items
    sw   ra, 4(sp)      # save return address
    sw   a0, 0(sp)      # save argument n
    addi t0, a0, -1     # t0 = n - 1
    bge  t0, x0, L1     # if (n - 1) >= 0, go to L1
    addi a0, x0, 1      # if (n - 1) < 0, return 1
    addi sp, sp, 8      # pop 2 items off stack
    jr   ra             # return to caller
    
L1:
    addi, a0, a0, -1    # n >= 1, argument gets (n - 1)
    jal   ra, factorial # call factorial with (n - 1)
    # next line is where the factorial function returns
    addi  t1, a0, 0     # t1 = a0, return from jal: move result of fact (n - 1) to t1
    lw    a0, 0(sp)     # restore argument n
    lw    ra, 4(sp)     # restore the return address
    addi  sp, sp, 8     # adjust stack pointer to pop 2 items
    mul   a0, a0, t1    # return n * factorial(n - 1)
    jalr  x0, ra, 0     # return to the caller 
