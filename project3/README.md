# CS61CPU

It's strange I can't pass the `test_integration_mem` test. It drives me crazy!

My custom Mem-type checker also occurs errors at specific scenarios. My design can pass the following test. However, if I remove the `lw t1, 0(sp)`, the self designed CPU will get trapped.

```asm
addi s0, x0, 1
addi s1, x0, 256

addi sp, sp, -12
sw s0, 0(sp)
sh s1, 4(sp)
sb s1, 8(sp)

lw t0, 0(sp)
lw t1, 0(sp)
lh t1, 0(sp)
lb t2, 8(sp)
addi sp, sp, 12
```

I don't want to waste too much time on this project which is related to the hardware, though this is a inappropriate behavior in study. I think it is a though task to dig out the potential mistakes in the dazzling circuits.

_However, other modules can pass all the tests without any error info!_

Much to my regret. :bug:
