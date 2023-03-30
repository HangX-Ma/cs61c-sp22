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
#       s1, argument pointer (a1)
#
#       s2, m0 matrix address
#       s11, m0 rows address
#       s10, m0 columns address
#
#       s3, m1 matrix address
#       s9, m1 rows address
#       s8, m1 columns address
#
#       s4, input matrix address
#       s7, input rows address
#       s6, input columns address


classify:
    # Prologue
    addi    sp, sp, -64
    sw      s0, 12(sp)
    sw      s1, 16(sp)
    sw      s2, 20(sp)
    sw      s3, 24(sp)
    sw      s4, 28(sp)
    sw      s5, 32(sp)
    sw      s6, 36(sp)
    sw      s7, 40(sp)
    sw      s8, 44(sp)
    sw      s9, 48(sp)
    sw      s10, 52(sp)
    sw      s11, 56(sp)
    sw      ra, 60(sp)
    # Epilogue

    # argument number checker
    li      s0, 5
    bne     a0, s0, argnum_error

    mv      s1, a1 # store the argument pointer
    mv      s0, a2 # store print switcher
    
    # Read pretrained m0, locate at address (a1 + 4)
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of rows of m0
    beqz    a0, malloc_error
    mv      s11, a0 # TODO (NEED FREE) s11 stores the m0's rows pointer address
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of column of m0
    beqz    a0, malloc_error
    mv      s10, a0 # TODO (NEED FREE) s10 stores the m0's column pointer address
    # read matrix
    lw      a0, 4(s1) # a[1]
    mv      a1, s11 # rows address
    mv      a2, s10 # columns address
    jal     ra, read_matrix
    mv      s2, a0 # TODO (NEED FREE) s2 stores m0's matrix address

	# Read pretrained m1, locate at address (a1 + 8)
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of rows of m1
    beqz    a0, malloc_error
    mv      s9, a0 # TODO (NEED FREE) s9 stores the m1's rows pointer address
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of column of m1
    beqz    a0, malloc_error
    mv      s8, a0 # TODO (NEED FREE) s8 stores the m1's column pointer address
    # read matrix
    lw      a0, 8(s1) # a[2]
    mv      a1, s9 # rows address
    mv      a2, s8 # columns address
    jal     ra, read_matrix
    mv      s3, a0 # TODO (NEED FREE) s3 stores m1's matrix address

	# Read input matrix, locate at address (a1 + 12)
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of rows of input
    beqz    a0, malloc_error
    mv      s7, a0 # TODO (NEED FREE) s7 stores the input's rows pointer address
    li      a0, 4
    jal     ra, malloc  # return the pointer to the num of column of input
    beqz    a0, malloc_error
    mv      s6, a0 # TODO (NEED FREE) s6 stores the input's column pointer address
    # read matrix
    lw      a0, 12(s1) # a[3]
    mv      a1, s7 # rows address
    mv      a2, s6 # columns address
    jal     ra, read_matrix
    mv      s4, a0 # TODO (NEED FREE) s4 stores input's matrix address

	# Compute h = matmul(m0, input)
    lw      t0, 0(s11) # m0 rows
    lw      t3, 0(s6) # input columns 
    # malloc
    mul     t4, t0, t3 # 'h' size (s11,s10)x(s7,s6) = (s11,s6)
    sw      t4, 8(sp)
    slli    a0, t4, 2 # 4-bytes per element
    jal     ra, malloc
    beqz    a0, malloc_error
    sw      a0, 0(sp) # TODO (NEED FREE) 'h'

    # matmul
    lw      a6, 0(sp) # store result in 'h'
    mv      a0, s2
    lw      a1, 0(s11)
    lw      a2, 0(s10)
    mv      a3, s4
    lw      a4, 0(s7)
    lw      a5, 0(s6)
    jal     ra, matmul


	# Compute h = relu(h)
    lw      a0, 0(sp) # 'h'
    lw      a1, 8(sp) # 'h' size
    jal     ra, relu

	# Compute o = matmul(m1, h)
    lw      t0, 0(s9) # m1 rows
    lw      t3, 0(s6) # 'h' columns
    # malloc
    mul     t4, t0, t3 # output size (s9,s8)x(s11,s6) = (s9,s6)
    sw      t4, 8(sp)
    slli    a0, t4, 2 # 4-bytes per element
    jal     ra, malloc
    beqz    a0, malloc_error
    sw      a0, 4(sp) # TODO (NEED FREE) 'o'

    # matmul
    lw      a6, 4(sp)
    mv      a0, s3
    lw      a1, 0(s9)
    lw      a2, 0(s8)
    lw      a3, 0(sp)
    lw      a4, 0(s11)
    lw      a5, 0(s6)
    jal     ra, matmul

	# Write output matrix o
    lw      a0, 16(s1) # a[4]
    lw      a1, 4(sp)
    lw      a2, 0(s9)
    lw      a3, 0(s6)
    jal     ra, write_matrix

	# Compute and return argmax(o)
    lw      a0, 4(sp)
    lw      a1, 8(sp)
    jal     ra, argmax
    sw      a0, 8(sp) # save return value of argmax

	# If enabled, print argmax(o) and newline
    bne     s0, x0, done
    lw      a0, 8(sp)
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

    mv      a0, s11
    jal     ra, free
    
    mv      a0, s10
    jal     ra, free

    mv      a0, s9
    jal     ra, free

    mv      a0, s8
    jal     ra, free

    mv      a0, s7
    jal     ra, free

    mv      a0, s6
    jal     ra, free

    # return
    lw      a0, 8(sp)

    lw      s0, 12(sp)
    lw      s1, 16(sp)
    lw      s2, 20(sp)
    lw      s3, 24(sp)
    lw      s4, 28(sp)
    lw      s5, 32(sp)
    lw      s6, 36(sp)
    lw      s7, 40(sp)
    lw      s8, 44(sp)
    lw      s9, 48(sp)
    lw      s10, 52(sp)
    lw      s11, 56(sp)
    lw      ra, 60(sp)
    addi    sp, sp, 64

	ret

malloc_error:
    li      a0, 26
    j       exit

argnum_error:
    li      a0, 31
    j       exit