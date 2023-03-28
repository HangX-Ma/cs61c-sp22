.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Registers:
#   t0, iteration counter
#   t1, memory address, fisrt array
#   t2, memory address, second array
#   t3, first array element at specific index
#   t4, second array elemnt at specific index
#   t5, sum
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
    # Prologue
    li      t0, 1 # error detection
    blt     a2, t0  array_len_err # The length of either array is less than 1
    blt     a3, t0, stride_len_err # The stride of either array is less than 1
    blt     a4, t0, stride_len_err
    mv      t0, x0 # reset counter to zero
    mv      t5, x0 # reset sum to zero
loop:
    # process fisrt array
    mul     t1, a3, t0 # step stride
    slli    t1, t1, 2 # 4 bytes per int
    add     t1, a0, t1 # memory address
    lw      t3, 0(t1) # first array element
    # process second array
    mul     t2, a4, t0 # step stride
    slli    t2, t2, 2 # 4 bytes per int
    add     t2, a1, t2 # memory address
    lw      t4, 0(t2) # first array element
    # sum
    mul     t3, t3, t4 # reuse t3 to store the mul result
    add     t5, t3, t5 # sum
    addi    t0, t0, 1
    bge     t0, a2, done # if t0 >= a2 then done
    j       loop
array_len_err:
    li      a0, 36
    j       exit
stride_len_err:
    li      a0, 37
    j       exit
    # Epilogue
done:
    mv      a0, t5
    ret
