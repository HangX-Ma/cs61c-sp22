#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    // YOUR CODE HERE
    int bias_x, bias_y;
    int block_num = n / blocksize + 1;
    
    for (int row = 0; row < block_num; ++row) {
        for (int col = 0; col < block_num; ++col) {
            for (int x = 0; x < blocksize; ++x) {
                for (int y = 0; y < blocksize; ++y) {
                    bias_x = x + col * blocksize;
                    bias_y = y + row * blocksize;
                    if (bias_x >= n || bias_y >= n) {
                        continue;
                    }
                    if (bias_x >= n && bias_y >= n) {
                        return;
                    }
                    dst[bias_y + bias_x * n] = src[bias_x + bias_y * n];
                }
            }
        }
    }
}
