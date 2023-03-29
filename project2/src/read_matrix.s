.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================

# USE venus SETTING!!!
# (venus environment calls) https://github.com/ThaumicMekanism/venus/wiki/Environmental-Calls
read_matrix:
    # Prologue
    addi    sp, sp, -32
    sw      ra, 4(sp)
    sw      s0, 8(sp)
    sw      s1, 12(sp)
    sw      s2, 16(sp)
    sw      s3, 20(sp)
    sw      s4, 24(sp)
    sw      s5, 28(sp)


    mv      s1, a1  # s1 saves the copy of a1
    mv      s2, a2  # s2 saves the copy of a2
    li      a1, 0  # read-only
    jal     ra, fopen

    li      t0, -1
    beq     a0, t0, fopen_error
    sw      a0, 0(sp)  # store the file pointer (fp)
    mv      a1, s1
    li      s3, 4
    mv      a2, s3
    jal     ra, fread
    bne     a0, s3, fread_error

    mv      a1, s2
    mv      a2, s3
    lw      a0, 0(sp)  # restore the file pointer (fp)
    jal     ra, fread
    bne     a0, s3, fread_error

    lw      t1, 0(s1)  # t1 = the number of rows
    lw      t2, 0(s2)  # t2 = the number of columns
    mul     s4, t1, t2
    slli    s4, s4, 2  # the size of the memory to be allocated
    mv      a0, s4
    jal     ra, malloc
    beqz    a0, malloc_error

    mv      s5, a0  # s5 = the address of the allocated memory
    lw      a0, 0(sp)  # file pointer
    mv      a1, s5
    mv      a2, s4
    jal     ra, fread
    bne     a0, s4, fread_error

    lw      a0, 0(sp)
    jal     ra, fclose
    bnez    a0, fclose_error

    mv      a0, s5

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

# === error handler ===
malloc_error:
    li      a0, 26
    j       exit

fopen_error:
    li      a0, 27
    j       exit

fclose_error:
    li      a0, 28
    j       exit

fread_error:
    li      a0, 29
    j       exit

matrix_size_error:
    li      a0, 38
    j       exit