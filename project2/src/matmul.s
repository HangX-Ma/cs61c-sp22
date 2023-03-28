.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*)  is the pointer to the start of m0
#   a1 (int)   is the # of rows (height) of m0
#   a2 (int)   is the # of columns (width) of m0
#   a3 (int*)  is the pointer to the start of m1
#   a4 (int)   is the # of rows (height) of m1
#   a5 (int)   is the # of columns (width) of m1
#   a6 (int*)  is the pointer to the the start of d
# Registers:
#   s0, column number
#   t0, row number
#   t1, inner counter
#   t2, outer, memory pointer
#   t3, inner/outer, memory address
#   t4, outer element
#   t5, inner element
#   t6, vec sum
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 38
# =======================================================
matmul:
    # Error checks
    li      t0, 1
    blt     a1, t0, error
    blt     a2, t0, error
    blt     a4, t0, error
    blt     a5, t0, error
    bne     a2, a4, error
    # Prologue
    addi    sp, sp, -4
    sw      s0, 0(sp)
    # init
    mv      t0, x0 # set row number to zero
    mv      s0, x0 # set column number to zero
    mv      t6, x0 
outer_loop:
    mv      t1, x0 # set inner counter to zero
    mv      t6, x0 # sum
    mul     t2, a2, t0 # get row memory bias
    slli    t2, t2, 2 # 4 bytes per element
    add     t2, a0, t2 # get outer pointer address
inner_loop:
    # access m1 element
    mul     t3, a5, t1 # step stride
    add     t3, t3, s0 # column bias added
    slli    t3, t3, 2 # 4 bytes per element
    add     t3, a3, t3 # get inner element address
    lw      t5, 0(t3)
    # access m0 element
    slli    t3, t1, 2 # 4 bytes per element
    add     t3, t2, t3 # get outer element address
    lw      t4, 0(t3)
    mul     t4, t5, t4 # store result in t4
    # store element
    add     t6, t6, t4
    # check inner loop condition
    addi    t1, t1, 1
    bge     t1, a2, inner_end
    j       inner_loop
inner_end:
    # get bias for output pointer
    mul     t4, a5, t0
    slli    t4, t4, 2
    # get current access memory address
    slli    t5, s0, 2
    add     t5, a6, t5
    add     t5, t5, t4 # add bias
    sw      t6, 0(t5)
    # reset temp sum
    mv      t6, x0
    # check if need clear column number
    addi    s0, s0, 1
    bge     s0, a5, clear_col_num
    j       outer_loop
clear_col_num:
    mv      s0, x0
outer_end:
    # check outer loop condition
    addi    t0, t0, 1
    bge     t0, a1, out
    j       outer_loop
out:
    lw      s0, 0(sp)
    addi    sp, sp, 4
    ret
error:
    li      a0, 38
    j       exit
