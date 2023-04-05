addi t0, x0, 3
addi t1, x0, 1
addi t2, x0, -2

start:
    beq t0, t1, end 
    bne t0, t1, p2
p1:
    bge t0, t1, p3
    blt t0, t1, p3
p2:
    bgeu t1, t0, p1
p3:
    bltu t0, t2, end

end: