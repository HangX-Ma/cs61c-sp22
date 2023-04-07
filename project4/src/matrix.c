#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

#define BASIC_OPERATION (0)
#define SIMD_OPERATION  (1)
#define OMP_OPERATION   (0)

/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Returns the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid. Note that the matrix is in row-major order.
 */
double get(matrix *mat, int row, int col) {
    // Task 1.1 TODO
    return *(mat->data + row * mat->cols + col);
}

/*
 * Sets the value at the given row and column to val. You may assume `row` and
 * `col` are valid. Note that the matrix is in row-major order.
 */
void set(matrix *mat, int row, int col, double val) {
    // Task 1.1 TODO
    *(mat->data + row * mat->cols + col) = val;
}

/*
 * Allocates space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set to NULL to indicate that
 * this matrix is not a slice. You should also set `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    // Task 1.2 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    // 3. Allocate space for the matrix data, initializing all entries to be 0. Return -2 if allocating memory failed.
    // 4. Set the number of rows and columns in the matrix struct according to the arguments provided.
    // 5. Set the `parent` field to NULL, since this matrix was not created from a slice.
    // 6. Set the `ref_cnt` field to 1.
    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if (rows < 1 || cols < 1) {
        return -1;
    }

    matrix* new_mat = (matrix *)malloc(sizeof(matrix));
    if (new_mat == NULL) {
        return -2;
    }

    new_mat->data = (double *)calloc(rows * cols, sizeof(double));
    if (new_mat->data == NULL) {
        return -2;
    }

    new_mat->rows = rows;
    new_mat->cols = cols;
    new_mat->parent = NULL;
    new_mat->ref_cnt = 1;

    *mat = new_mat;

    return 0;
}

/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or that you free `mat->parent->data` if `mat` is the last existing slice of its parent matrix and its parent
 * matrix has no other references (including itself).
 */
void deallocate_matrix(matrix *mat) {
    // Task 1.3 TODO
    // HINTS: Follow these steps.
    // 1. If the matrix pointer `mat` is NULL, return.
    // 2. If `mat` has no parent: decrement its `ref_cnt` field by 1. If the `ref_cnt` field becomes 0, then free `mat` and its `data` field.
    // 3. Otherwise, recursively call `deallocate_matrix` on `mat`'s parent, then free `mat`.
    if (mat == NULL) {
        return;
    }

    if (mat->parent == NULL) {
        mat->ref_cnt--;

        if (mat->ref_cnt == 0) {
            free(mat->data);
            free(mat);
            mat->data = NULL;
            mat = NULL;
        }
    } else {
        deallocate_matrix(mat->parent);
        free(mat);
        mat = NULL;
    }

}

/*
 * Allocates space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set to `from` to indicate this matrix is a slice of `from`
 * and the reference counter for `from` should be incremented. Lastly, do not forget to set the
 * matrix's row and column values as well.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 * NOTE: Here we're allocating a matrix struct that refers to already allocated data, so
 * there is no need to allocate space for matrix data.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int offset, int rows, int cols) {
    // Task 1.4 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    // 3. Set the `data` field of the new struct to be the `data` field of the `from` struct plus `offset`.
    // 4. Set the number of rows and columns in the new struct according to the arguments provided.
    // 5. Set the `parent` field of the new struct to the `from` struct pointer.
    // 6. Increment the `ref_cnt` field of the `from` struct by 1.
    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if (rows < 1 || cols < 1) {
        return -1;
    }

    matrix *new_matrix = (matrix *)malloc(sizeof(matrix));
    if (new_matrix == NULL) {
        return -2;
    }

    new_matrix->data = from->data + offset;
    new_matrix->rows = rows;
    new_matrix->cols = cols;
    new_matrix->parent = from;
    from->ref_cnt++;

    *mat = new_matrix;

    return 0;
}

/*
 * Sets all entries in mat to val. Note that the matrix is in row-major order.
 */
void fill_matrix(matrix *mat, double val) {
    // Task 1.5 TODO
#if BASIC_OPERATION
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            set(mat, i, j, val);
        }
    }
#elif SIMD_OPERATION
    __m256d vec = _mm256_set1_pd(val);
    unsigned int truncated_mat_size = mat->rows * mat->cols / 4 * 4;
    for (int i = 0; i < truncated_mat_size; i += 4) {
        _mm256_storeu_pd(mat->data + i, vec);
    }

    for (int i = truncated_mat_size; i < mat->rows * mat->cols; i++) {
        *(mat->data + i) = val;
    }

#elif OMP_OPERATION
#endif

}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int abs_matrix(matrix *result, matrix *mat) {
    // Task 1.5 TODO
#if BASIC_OPERATION
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, abs(get(mat, i, j)));
        }
    }
#elif SIMD_OPERATION
    __m256d _zero = _mm256_set1_pd(0);
    unsigned int truncated_mat_size = mat->rows * mat->cols / 4 * 4;
    for (int i = 0; i < truncated_mat_size; i += 4) {
        __m256d _vec     = _mm256_loadu_pd(mat->data + i);
        __m256d _sub_vec = _mm256_sub_pd(_zero, _vec); 
        __m256d _abs_vec = _mm256_max_pd(_sub_vec, _vec);
        _mm256_storeu_pd(result->data + i, _abs_vec);
    }

    for (int i = truncated_mat_size; i < mat->rows * mat->cols; i++) {
        double _val = *(mat->data + i);
        if (_val < 0) {
            _val = -_val;
        }
        *(result->data + i) = _val;
    }
#elif OMP_OPERATION
#endif
    return 0;
}


/*
 * (OPTIONAL)
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int neg_matrix(matrix *result, matrix *mat) {
    // Task 1.5 TODO
#if BASIC_OPERATION
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, -get(mat, i, j));
        }
    }
#elif SIMD_OPERATION
    __m256d _zero = _mm256_set1_pd(0);
    unsigned int truncated_mat_size = mat->rows * mat->cols / 4 * 4;
    for (int i = 0; i < truncated_mat_size; i += 4) {
        __m256d _vec     = _mm256_loadu_pd(mat->data + i);
        __m256d _sub_vec = _mm256_sub_pd(_zero, _vec); 
        _mm256_storeu_pd(result->data + i, _sub_vec);
    }

    for (int i = truncated_mat_size; i < mat->rows * mat->cols; i++) {
        *(mat->data + i) = -*(mat->data + i);
    }
#elif OMP_OPERATION
#endif
    return 0;
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.5 TODO
#if BASIC_OPERATION
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, get(mat2, i, j) + get(mat1, i, j));
        }
    }
#elif SIMD_OPERATION
    unsigned int truncated_mat_size = result->cols / 4 * 4;
    for (int i = 0; i < truncated_mat_size; i += 4) {
        __m256d _vec1    = _mm256_loadu_pd(mat1->data + i);
        __m256d _vec2    = _mm256_loadu_pd(mat2->data + i);
        __m256d _add_vec = _mm256_add_pd(_vec1, _vec2);
        _mm256_storeu_pd(result->data + i, _add_vec);
    }

    for (int i = truncated_mat_size * result->rows; i < result->rows * result->cols; i++) {
        *(result->data + i) = *(mat1->data + i) + *(mat2->data + i);
    }
#elif OMP_OPERATION
#endif
    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.5 TODO
#if BASIC_OPERATION
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, get(mat2, i, j) - get(mat1, i, j));
        }
    }
#elif SIMD_OPERATION
    unsigned int truncated_mat_size = result->cols / 4 * 4;
    for (int i = 0; i < truncated_mat_size; i += 4) {
        __m256d _vec1    = _mm256_loadu_pd(mat1->data + i);
        __m256d _vec2    = _mm256_loadu_pd(mat2->data + i);
        __m256d _sub_vec = _mm256_sub_pd(_vec1, _vec2);
        _mm256_storeu_pd(result->data + i, _sub_vec);
    }

    for (int i = truncated_mat_size * result->rows; i < result->rows * result->cols; i++) {
        *(result->data + i) = *(mat1->data + i) - *(mat2->data + i);
    }
#elif OMP_OPERATION
#endif
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 * You may assume `mat1`'s number of columns is equal to `mat2`'s number of rows.
 * Note that the matrix is in row-major order.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.6 TODO
#if BASIC_OPERATION
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            int mul_sum = 0;
            for (int idx = 0; idx < result->cols; idx++) {
                mul_sum += get(mat1, i, idx) * get(mat2, idx, j);
            }
            set(result, i, j, mul_sum);
        }
    }
#elif SIMD_OPERATION
    // transpose the 2nd matrix
    matrix *mat2_tr = NULL;
    allocate_matrix(&mat2_tr, mat2->cols, mat2->rows);
    for (int i = 0; i < mat2->rows; i++) {
        for (int j = 0; j < mat2->cols; j++) {
            set(mat2_tr, j, i, get(mat2, i, j));
        }
    }

    /* Calculate dot product */
    double sum_arr[4];
    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat2_tr->rows; j++) {
            double sum = 0.0;
            int k = 0;
            unsigned int truncated_cols_size = mat2_tr->cols / 4 * 4;
            for (; k < truncated_cols_size; k += 4) {
                __m256d _vec1    = _mm256_loadu_pd(mat1->data + i * mat1->cols + k);
                __m256d _vec2    = _mm256_loadu_pd(mat2_tr->data + j * mat2_tr->cols + k);
                __m256d _mul_vec = _mm256_mul_pd(_vec1, _vec2);
                _mm256_storeu_pd(sum_arr, _mul_vec);
                sum += sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3];
            }
            /* tail */
            for (; k < mat2_tr->cols; k++) {
                sum += get(mat1, i, k) * get(mat2_tr, j, k);
            }
            set(result, i, j, sum);
        }
    }
#elif OMP_OPERATION
#endif
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 * You may assume `mat` is a square matrix and `pow` is a non-negative integer.
 * Note that the matrix is in row-major order.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    // Task 1.6 TODO
    // Create matrix E
    if (pow == 0) {
        for (size_t i = 0; i < result->rows; i++) {
            set(result, i, i, 1);
        } 
    }

    matrix *zero_mat = NULL;
    allocate_matrix(&zero_mat, result->rows, result->rows);
    fill_matrix(zero_mat, 0);
    add_matrix(result, zero_mat, mat);

    if (pow == 1) {
        return 0;
    }

    matrix *tmp_mat = NULL;
    allocate_matrix(&tmp_mat, result->rows, result->rows);
    for (size_t i = 0; i < pow - 1; i++) {
        mul_matrix(tmp_mat, result, mat);
        add_matrix(result, zero_mat, tmp_mat);
    }

    return 0;
}
