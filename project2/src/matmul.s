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
#   t0, outer counter
#   t1, inner counter
#   t2, outer, memory address
#   t3, inner, memory address
#   t4, outer element
#   t5, inner element
#   t6, total element counter
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
    mv      t0, x0 # set outer counter to zero
    mv      t6, x0 # total element
outer_loop:
    mv      t1, x0 # set inner counter to zero
    mul     t2, t2, a2 # get outer memory bias
    slli    t2, t2, 2 # 4 bytes per element
    add     t2, a0, t2 # get outer pointer address
inner_loop:
    # access inner element
    mul     t3, a4, t1 # step stride
    slli    t3, t3, 2 # 4 bytes per element
    add     t3, a3, t3 # get inner element address
    lw      t5, 0(t3)
    # access outer element
    slli    t3, t1, 2 # 4 bytes per element
    add     t3, t2, t3 # get outer element address
    lw      t4, 0(t3)
    mul     t4, t5, t4 # store result in t4
    # store element
    addi    t6, t6, # TODO : NEED STRIDE
    slli    t6, t6, 2
    addi    t5, t6, a6

    # check inner loop condition
    addi    t1, t1, 1
    bge     t1, a2, inner_end

inner_end:
    # check outer loop condition
    addi    t0, t0, 1
    bge     t0, a1, outer_end
    j       outer_loop
outer_end:
    # Epilogue
    ret

error:
    li      a0, 38
    j       exit
