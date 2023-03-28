.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Registers:
#   t0, index of array
#   t1, memory location of an specific element
#   t2, element at ceratin index
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
	# Prologue
    li      t0, 1 # set cmp length t0 1
    blt     a1, t0, exception # less than 1, raise exception
    mv      t0, x0 # reset index to zero 
loop:
    slli    t1, t0, 2 # int type require 4 bytes space
    add     t1, a0, t1
    lw      t2, 0(t1) # get element
    blt     t2, x0, set_zero # if t2 < 0 then set_zero
next_element:
    addi    t0, t0, 1
    blt     t0, a1, loop
    j       done
set_zero:
    sw      x0, 0(t1) # set zero
    j       next_element
exception:
    li      a0, 36
    j       exit
done:
	# Epilogue
	ret
