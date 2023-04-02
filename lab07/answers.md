# Lab 7: Caches Answers

I use the questions found on [fa21](https://inst.eecs.berkeley.edu/~cs61c/fa21/labs/lab07/#exercise-1-memory-accesses) for the same exercise. (Google forum needs validation)

## Exercise1

### Scenario 1

1. Because step size in bytes is exactly equal to block size in bytes. (All accesses get compulsory miss)
2. 0.0. Because we only change the replication times, rate will not change.
3. Change step size to 1 will increase the hit rate to 50%. (Notice that one element size is 4 bytes! Step bytes >= Block size will reduce the hit rate to zero!)

### Scenario 2

1. 2 memory accesses per iteration.
2. Miss, Hit, Hit, Hit (mhhh).
3. I set rep count to 1024 and the `Hit Rate` increase to `0.999755859375` which is nearly approach to `1.0`. (Hint: Cache Size = Array Size)
4. We should try to access **each part whose size is equal to cache size** of the array at a time and apply all of the **functions** to that **part** so we can be completely done with it before moving on, thereby keeping that **part** hot in the cache and not having to circle back to it later on!

### Scenario 3

1. The hit rate of L1 is 50%, L2 is 0, Overall is 50%. (If L1 not hit, it will access L2. If L2 not hit, it will access the main memory. Therefore the overall hit rate is the same as L1 hit rate on account of hit rate 0 of L2)
2. 32 accesses to L1 total. 16 of them are misses.
3. 16 accesses to L2 total.
4. Increase the **Rep Count** in program parameters. (I set **Rep Count** to 100 and the L2 hit rate increase to 99%)
5. (1) L1 and L2 stay the same; (2) L1 hit rate increases but L2 stay the same.

## Exercise 2

```txt
ijk:    n = 1000, 0.885 Gflop/s
ikj:    n = 1000, 0.096 Gflop/s
jik:    n = 1000, 0.899 Gflop/s
jki:    n = 1000, 8.750 Gflop/s
kij:    n = 1000, 0.099 Gflop/s
kji:    n = 1000, 7.402 Gflop/s
```

1. jki performs the best.
2. ikj performs the worst.
The more stride, the worser.

## Exercise 3

### Part 1 - Changing Array Sizes

My CPU Property

```txt
L1d cache:                       128 KiB
L1i cache:                       128 KiB
L2 cache:                        1 MiB
L3 cache:                        6 MiB
```

|block size|n|native/ms|student/ms|
|:--:|:--:|:--:|:--:|
|20|100|0.005|0.024|
|20|1000|1.026|2.279|
|20|2000|24.569|8.037|
|20|5000|208.949|54.411|
|20|10000|1271|225.363|
|20|20000|8482.34|1171.93|

My test doesn't run on hive machine. With `L1d cache` size 128KiB, that means I can create a matrix whose element can reach to 32 Ki. This makes the test out of time.

> Why does cache blocking require the matrix to be a certain size before it outperforms the non-cache blocked code?

Here's the answer provided by the TA.

_**TA:**_ With a bigger matrix size, parts of the matrix can't fit inside the cache, leading to more cache misses from non-blocked code. Blocked code would consolidate accesses to a smaller portion of memory, giving more cache hits. With a smaller matrix size, most of the matrix can probably fit in the cache, so blocking results in more overhead (for loop exits) and doesn't provide a significant increase in cache hit rate.

### Part 2 - Changing Block Size

|block size|n|native/ms|student/ms|
|:--:|:--:|:--:|:--:|
|50|10000|1022.11|209.942|
|100|10000|948.743|266.923|
|500|10000|1246.24|233.651|
|1000|10000|1249|274.217|
|5000|10000|1262.03|942.472|
|10000|10000|1035.2|1428.44|

> How does performance change as blocksize increases? Why is this the case?

The performance continuously decrease on my machine.
Actually on my machine, the performance decrease may result from the cache rate decrease.

_**TA:**_ When the blocksize is too large, the entire block cannot fit inside the cache, negating the advantage of cache blocking. In fact, if the blocksize keeps increasing, we just arrive at the naive approach again.
