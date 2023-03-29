.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fwrite error or eof,
#     this function terminates the program with error code 30
# ==============================================================================
write_matrix:

	# Prologue
    addi    sp, sp, -32
    sw      ra, 4(sp)
    sw      s0, 8(sp)
    sw      s1, 12(sp)
    sw      s2, 16(sp)
    sw      s3, 20(sp)
    sw      s4, 24(sp)
    sw      s5, 28(sp)

    mv      s0, a0
    mv      s1, a1
    mv      s2, a2
    mv      s3, a3

    li      a1, 1 # write only
    jal     ra, fopen
    blt     a0, x0, fopen_error
    mv      s5, a0 # store the file pointer (fp)

    # write rows
    sw      s2, 0(sp) # store rows into the memory
    mv      a0, s5
    mv      a1, sp
    li      s4, 1
    mv      a2, s4 # element number
    li      a3, 4 # size per element
    jal     ra, fwrite
    bne     a0, s4, fwrite_error
    
    # write column
    sw      s3, 0(sp) # store rows into the memory
    mv      a0, s5
    mv      a1, sp
    li      s4, 1
    mv      a2, s4 # element number
    li      a3, 4 # size per element
    jal     ra, fwrite
    bne     a0, s4, fwrite_error

    # write data
    mul     s4, s2, s3 # get total element number
    mv      a0, s5
    mv      a1, s1
    mv      a2, s4
    li      a3, 4
    jal     ra, fwrite
    bne     a0, s4, fwrite_error

    mv      a0, s5
    jal     ra, fclose
    bnez    a0, fclose_error

	# Epilogue
    lw      ra, 4(sp)
    lw      s0, 8(sp)
    lw      s1, 12(sp)
    lw      s2, 16(sp)
    lw      s3, 20(sp)
    lw      s4, 24(sp)
    lw      s5, 28(sp)
    addi    sp, sp, 32


	ret

fopen_error:
    li      a0, 27
    j       exit

fclose_error:
    li      a0, 28
    j       exit

fwrite_error:
    li      a0, 30
    j       exit