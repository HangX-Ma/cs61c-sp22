# 61C Spring 2022 Project 1: Snake

Spec: [https://cs61c.org/sp22/projects/proj1/](https://cs61c.org/sp22/projects/proj1/)

I make some tiny mistakes that cause the program encounter weird problems:

- Forget to index the snakes `state->snake->live`, which makes only first snake alive.
- Wrongly use the function pointer. I actually call the `add_food` function.
- I write `col` instead of `cow` in two dimensions array dynamic memory allocation, which leads to illegal out of range access.

The thing that gives me the most headache is the memory management. I free a continuous memory for several times for _snakes_. This problem was found by using `Valgrind` memory checker tool. During the debugging period, `cgdb` and `Valgrind` assisted me at figuring out logic and input errors.

> eg. illegal memory release

```c
for (int i = 0; i < state->snake_nums; ++i) {
    free(state->snakes + i);
}
```

This project helps me get familiar with `Valgrind` and `cgdb`. Those similar assistant tools can make great efficiency increment. I also learned how to write some simple test program!

Though my initial purpose is studying RISC-V architecture, the fundamental labs and projects about C language really borden my horizon and practice my capability to code.

Thanks!

2023.03 :tada:
