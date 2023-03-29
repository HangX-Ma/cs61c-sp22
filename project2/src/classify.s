.globl classify

.text
# =====================================
# COMMAND LINE ARGUMENTS
# =====================================
# Args:
#   a0 (int)        argc
#   a1 (char**)     argv
#   a1[1] (char*)   pointer to the filepath string of m0
#   a1[2] (char*)   pointer to the filepath string of m1
#   a1[3] (char*)   pointer to the filepath string of input matrix
#   a1[4] (char*)   pointer to the filepath string of output file
#   a2 (int)        silent mode, if this is 1, you should not print
#                   anything. Otherwise, you should print the
#                   classification and a newline.
# Returns:
#   a0 (int)        Classification
# Exceptions:
#   - If there are an incorrect number of command line args,
#     this function terminates the program with exit code 31
#   - If malloc fails, this function terminates the program with exit code 26
#
# Usage:
#   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>
#
# Registers:
#       s1, argument pointer
#
#       s2, m0 matrix address
#       t1, m0 rows
#       t2, m0 columns
#
#       s3, m1 matrix address
#       t3, m1 rows
#       t4, m1 columns
#
#       s4, input matrix address
#       t5, input rows
#       t6, input columns



classify:
    # Prologue
    addi    sp, sp, -36
    sw      s0, 8(sp)
    sw      s1, 12(sp)
    sw      s2, 16(sp)
    sw      s3, 20(sp)
    sw      s4, 24(sp)
    sw      s5, 28(sp)
    sw      ra, 32(sp)

    # argument number checker
    li      s0, 5
    bne     a0, s0, argnum_error

    mv      s1, a1 # store the argument pointer
    mv      s0, a2 # store print switcher

	# Read pretrained m0, locate at address (a1 + 4)
    lw      a0, 4(s1)
    mv      a1, sp # rows
    addi    a2, sp, 4 # columns
    jal     ra, read_matrix
    mv      s2, a0 # m0 matrix address
    lw      t1, 0(sp)
    lw      t2, 4(sp)

	# Read pretrained m1, locate at address (a1 + 8)
    lw      a0, 8(s1)
    mv      a1, sp # rows
    addi    a2, sp, 4 # columns
    jal     ra, read_matrix
    mv      s3, a0 # m1 matrix address
    lw      t3, 0(sp)
    lw      t4, 4(sp)

	# Read input matrix, locate at address (a1 + 12)
    lw      a0, 12(s1)
    mv      a1, sp # rows
    addi    a2, sp, 4 # columns
    jal     ra, read_matrix
    mv      s3, a0 # input matrix address
    lw      t5, 0(sp)
    lw      t6, 4(sp)

	# Compute h = matmul(m0, input)
    # malloc
    mul     t0, t1, t6 # output size (t1,t2)x(t5,t6) = (t1,t6)
    slli    a0, t0, 2 # 4-bytes per element
    jal     ra, malloc
    beqz    a0, malloc_error
    mv      s5, a0 # store the pointer h
    sw      s5, 0(sp) # h, prepare for free
    # matmul
    mv      a6, a0
    mv      a0, s2
    mv      a1, t1
    mv      a2, t2
    mv      a3, s4
    mv      a4, t5
    mv      a5, t6
    jal     ra, matmul

	# Compute h = relu(h)
    mv      a0, s5 # h
    mv      a1, t0 # output size
    jal     ra, relu

	# Compute o = matmul(m1, h)
    # malloc
    mul     t0, t3, t6 # output size (t3,t4)x(t1,t6) = (t3,t6)
    slli    a0, t0, 2 # 4-bytes per element
    jal     ra, malloc
    beqz    a0, malloc_error
    mv      s5, a0 # store the pointer o
    sw      s5, 4(sp) # o, prepare for free
    # matmul
    mv      a6, a0
    mv      a0, s3
    mv      a1, t3
    mv      a2, t4
    mv      a3, s5
    mv      a4, t1
    mv      a5, t6
    jal     ra, matmul

	# Write output matrix o
    lw      a0, 16(s1)
    mv      a1, s5
    mv      a2, t3
    mv      a3, t6
    jal     ra, write_matrix

	# Compute and return argmax(o)
    mv      a0, s5
    mv      a1, t0
    jal     ra, argmax
    mv      t4, a0 # save return value of argmax

	# If enabled, print argmax(o) and newline
    bne     s0, a2, done
    mv      a0, t4
    jal     ra, print_int

    # finish
    li      a0, '\n'
    jal     ra, print_char
done:
    # free pointer 'h'
    lw      a0, 0(sp)
    jal     ra, free
    # free pointer 'o'
    lw      a0, 4(sp)
    jal     ra, free
    # free matrix m0
    mv      a0, s2
    jal     ra, free
    # free matrix m1
    mv      a0, s3
    jal     ra, free
    # free matrix input
    mv      a0, s4
    jal     ra, free

    mv      a0, t4

    lw      s0, 8(sp)
    lw      s1, 12(sp)
    lw      s2, 16(sp)
    lw      s3, 20(sp)
    lw      s4, 24(sp)
    lw      s5, 28(sp)
    lw      ra, 32(sp)
    addi    sp, sp, 36

	ret

malloc_error:
    li      a0, 26
    j       exit

argnum_error:
    li      a0, 31
    j       exit