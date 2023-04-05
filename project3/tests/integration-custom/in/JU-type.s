    jal x0, p2

p1:
    auipc t0, end 
    j end
p2:
    jalr x0, x0, p1
end: