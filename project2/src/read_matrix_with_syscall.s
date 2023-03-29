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
# Registers:
#   s0, cache aaddres in a0 
#   s1, cache address in a1
#   s2, cache address in a2
#   t0, common usage
#   t5, store the file descriptor in t5
#   t6, 4-bytes size buffer
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
    addi    sp, sp, -16
    sw      s0, 0(sp)
    sw      s1, 4(sp)
    sw      s2, 8(sp)
    sw      ra, 12(sp)

    mv      s0, a0
    mv      s1, a1
    mv      s2, a2

    # malloc 4 byte buffer
    li      a0, 9 # sbrk ID=9, allocate a1 bytes on the heap
    li      a1, 4 # 4 bytes
    ecall
    beqz    a0, malloc_error
    mv      t6, a0 # store the buffer pointer in t6

    # open_file
    li      a0, 13 # openFile ID=13
    mv      a1, s0 # a1 = Path
    mv      a2, x0 # a2 = permission, O_RDONLY
    ecall
    blt     a0, x0, fopen_error # error will return -1
    mv      t5, a0 # store the file descriptor in t5

    # ----------- Now we can reuse s0 -----------
    # get row count
    jal     ra, fread_func
    lw      a0, 0(a2) # read buffer data
    beqz    a0, matrix_size_error
    sw      a0, 0(s1) # store row count in s1(initial a1)
    # prepare for multiplication, s0 = row
    mv      s0, a0
    # get column count
    jal     ra, fread_func
    lw      a0, 0(a2)
    beqz    a0, matrix_size_error
    sw      a0, 0(s2)

    # ------------ Now we can reuse s1 and s2 ------------
    # get total element number, stored in s1
    mul     s1, a0, s0
    mv      s0, x0 # reset s0 to 0, use s0 as the counter, s1 the sum

    # malloc enough space to store the element
    li      a0, 9 # sbrk ID=9, allocate a1 bytes on the heap
    mv      a1, s1
    slli    a1, a1, 2 # 4 bytes per element
    ecall
    beqz    a0, malloc_error
    mv      s2, a0 # s2 now become the temporary output pointer

loop:
    jal     ra, fread_func
    lw      a0, 0(a2)
    slli    t0, s0, 2
    add     t0, s2, t0
    sw      a0, 0(t0)
    # check break condition
    addi    s0, s0, 1
    bge     s0, s1, done
    j       loop

done:
    # close file
    li      a0, 16 # closeFile ID=16
    mv      a1, t5 # a1 = FileDescriptor
    ecall
    blt     a0, x0, fclose_error

    # change pointer owner
    mv      a0, s2

    # Epilogue
    lw      s0, 0(sp)
    lw      s1, 4(sp)
    lw      s2, 8(sp)
    lw      ra, 12(sp)
    addi    sp, sp, 16

    ret

# === function handler ===
fread_func:
    li      a0, 14 # readFile ID=14
    mv      a1, t5 # a1 = FileDescriptor
    mv      a2, t6 # a2 = Buffer to read data from
    li      a3, 4 # read 4 bytes
    ecall
    bne     a0, a3, fread_error
    jr      ra

# === error handler ===
malloc_error:
    # Epilogue
    li      a0, 26
    j       exit

fopen_error:
    # Epilogue
    li      a0, 27
    j       exit

fclose_error:
    # Epilogue
    li      a0, 28
    j       exit

fread_error:
    # Epilogue
    li      a0, 29
    j       exit

matrix_size_error:
    # Epilogue
    li      a0, 38
    j       exit