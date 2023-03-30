# 61C Spring 2022 Project 2: CS61Classify

Spec: [https://cs61c.org/sp22/projects/proj2/](https://cs61c.org/sp22/projects/proj2/)

Before your coding, it is highly recommended to look through the [Appendix: Calling Convention](https://inst.eecs.berkeley.edu/~cs61c/sp22/projects/proj2/calling-convention/). What's more, [Appendix: Function Definitions](https://inst.eecs.berkeley.edu/~cs61c/sp22/projects/proj2/function-definitions/) and the [venus - Environment Calls](https://github.com/ThaumicMekanism/venus/wiki/Environmental-Calls) provided in [Venus Reference](https://inst.eecs.berkeley.edu/~cs61c/sp22/resources/venus-reference/) will speed up your project. I once wasted a lot time to dig out the fundamental information for coding. Actually, the enthusiastic teachers have already provide them in their notes.

I finish the PartA well because they are simple modules that will be used to integrate together. However, when dealing with problems in PartB, on account of the violation of _Calling Convention_, I stuck into a trap that the venus always tell me the `"Usage of unset register t0"` warning. I undoubtedly initialize the register! Eventually I found the impressive error that I use the so-called **garbage** value. PartB asks you to call some functions and this in turn asks you to follow the _Calling Convention_.

I regret that I didn't read those materials carefully.

## Note

If you want to spare space for stack which can be easily accessed, you can do something like this:

```risc-v
    addi sp, sp, -16
    sw   s0, 8(sp)
    sw   s1, 12(sp)
```

Now you can easily access the `0(sp)` and `4(sp)`! :tada:
