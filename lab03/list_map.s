.globl map

.text
main:
    # create_default_list will create a list whose value from 0 to 9
    jal ra, create_default_list
    add s0, a0, x0 # a0 (and now s0) is the head of node list

    # Print the list
    add a0, s0, x0
    jal ra, print_list
    # Print a newline
    jal ra, print_newline

    # === Calling `map(head, &square)` ===
    # Load function arguments
    add a0, s0, x0 # Loads the address of the first node into a0

    # Load the address of the "square" function into a1 (hint: check out "la" on the green sheet)
    ### YOUR CODE HERE ###
    la a1, square

    # Issue the call to map
    jal ra, map

    # Print the squared list
    add a0, s0, x0
    jal ra, print_list
    jal ra, print_newline

    # === Calling `map(head, &decrement)` ===
    # Because our `map` function modifies the list in-place, the decrement takes place after
    # the square does

    # Load function arguments
    add a0, s0, x0 # Loads the address of the first node into a0

    # Load the address of the "decrement" function into a1 (should be very similar to before)
    ### YOUR CODE HERE ###
    la a1, decrement

    # Issue the call to map
    jal ra, map

    # Print decremented list
    add a0, s0, x0
    jal ra, print_list
    jal ra, print_newline

    addi a0, x0, 10
    ecall # Terminate the program

# a0 --> address of head node
# a1 --> function pointer
# s0 --> container of head node address
# s1 --> container of function address
# ra --> returned address
#
# We need to back up s0 and s1 using stack!
# All saved register we use need this operation.

map:
    # Prologue: Make space on the stack and back-up registers
    ### YOUR CODE HERE ###
    addi sp, sp, -12 # RISC32-I
    sw s0, 8(sp)
    sw s1, 4(sp) 
    sw ra, 0(sp)
next_node:
    beq a0, x0, done # If we were given a null pointer (address 0), we're done.

    add s0, a0, x0 # Save address of this node in s0
    add s1, a1, x0 # Save address of function in s1

    # Remember that each node is 8 bytes long: 4 for the value followed by 4 for the pointer to next.
    # What does this tell you about how you access the value and how you access the pointer to next?

    # Load the value of the current node into a0
    # THINK: Why a0? a0 is fisrt parameter of the function
    ### YOUR CODE HERE ###
    lw a0, 0(s0)

    # Call the function in question on that value. DO NOT use a label (be prepared to answer why).
    # Hint: Where do we keep track of the function to call? Recall the parameters of "map".

    # Ans: We don't know where the function come from before we link the assembled object into an
    # executable file. The specific function may be a unterpretted symbol if we use the label.
    ### YOUR CODE HERE ###
    jalr ra, s1, 0

    # Store the returned value back into the node
    # Where can you assume the returned value is? Ans: a0
    ### YOUR CODE HERE ###
    sw a0, 0(s0)

    # Load the address of the next node into a0
    # The address of the next node is an attribute of the current node.
    # Think about how structs are organized in memory.
    ### YOUR CODE HERE ###
    lw a0, 4(s0)

    # Put the address of the function back into a1 to prepare for the recursion
    # THINK: why a1? What about a0?
    ### YOUR CODE HERE ###
    mv a1, s1

    # Recurse
    ### YOUR CODE HERE ###
    j next_node
done:
    # Epilogue: Restore register values and free space from the stack
    ### YOUR CODE HERE ###
    lw s0, 8(sp)
    lw s1, 4(sp) 
    lw ra, 0(sp)
    addi sp, sp, 12 # RISC32-I

    jr ra # Return to caller

# === Definition of the "square" function ===
square:
    mul a0, a0, a0
    jr ra

# === Definition of the "decrement" function ===
decrement:
    addi a0, a0, -1
    jr ra

# === Helper functions ===
# You don't need to understand these, but reading them may be useful

create_default_list:
    addi sp, sp, -12    # adjust stack for 2 items
    sw ra, 0(sp)        # store the return address
    sw s0, 4(sp)
    sw s1, 8(sp)
    li s0, 0            # Pointer to the last node we handled
    li s1, 0            # Number of nodes handled
loop:                   # do...
    li a0, 8            #     Allocate 8 bytes, int and pointer are both 4 byte each
    jal ra, malloc      #     Allocate memory for the next node
    sw s1, 0(a0)        #     node->value = i
    sw s0, 4(a0)        #     node->next = last
    add s0, a0, x0      #     last = node
    addi s1, s1, 1      #     i++
    addi t0, x0, 10
    bne s1, t0, loop    # ... while i!= 10
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    addi sp, sp, 12
    jr ra

print_list:
    bne a0, x0, print_me_and_recurse
    jr ra               # Nothing to print
print_me_and_recurse:
    add t0, a0, x0      # t0 gets current node address
    lw a1, 0(t0)        # a1 gets value in current node
    addi a0, x0, 1      # Prepare for print integer ecall
    ecall
    addi a1, x0, ' '    # a0 gets address of string containing space
    addi a0, x0, 11     # Prepare for print char syscall
    ecall
    lw a0, 4(t0)        # a0 gets address of next node
    jal x0, print_list  # Recurse. The value of ra hasn't been changed.

print_newline:
    addi a1, x0, '\n'   # Load in ascii code for newline
    addi a0, x0, 11
    ecall
    jr ra

malloc:
    # NOTE: venus uses a0 for simplicity instead of a7 register
    addi a1, a0, 0      # store the required memory size in a1 parameter
    addi a0, x0, 9      # a0 actually store the syscall sbrk(9)
    ecall
    jr ra               # return the allocated memory address in a0
