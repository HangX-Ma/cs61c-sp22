/* Include the system headers we need */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Include our header */
#include "vector.h"

/* Define what our struct is */
struct vector_t {
    size_t size;
    int *data;
};

/* Utility function to handle allocation failures. In this
   case we print a message and exit. */
static void allocation_failed(size_t line) {
    fprintf(stderr, "Out of memory at %ld.\n", line);
    exit(1);
}

/* Bad example of how to create a new vector */
vector_t *bad_vector_new() {
    /* Create the vector and a pointer to it */
    vector_t *retval, v;
    retval = &v;

    /* Initialize attributes */
    retval->size = 1;
    retval->data = malloc(sizeof(int));
    if (retval->data == NULL) {
        allocation_failed(__LINE__);
    }

    retval->data[0] = 0;
    return retval;
}

/* Another suboptimal way of creating a vector */
vector_t also_bad_vector_new() {
    /* Create the vector */
    vector_t v;

    /* Initialize attributes */
    v.size = 1;
    v.data = malloc(sizeof(int));
    if (v.data == NULL) {
        allocation_failed(__LINE__);
    }
    v.data[0] = 0;
    return v;
}

/* Create a new vector with a size (length) of 1 and set its single component to zero... the
   right way */
/* TODO: uncomment the code that is preceded by // */
vector_t *vector_new() {
    /* Declare what this function will return */
    vector_t *retval;

    /* First, we need to allocate memory on the heap for the struct */
    retval = (vector_t *)malloc(sizeof(vector_t));

    /* Check our return value to make sure we got memory */
    if (retval == NULL) {
        allocation_failed(__LINE__);
    }

    /* Now we need to initialize our data.
       Since retval->data should be able to dynamically grow,
       what do you need to do? */
    retval->size = 1/* YOUR CODE HERE */;
    retval->data = (int *)malloc(sizeof(int))/* YOUR CODE HERE */;

    /* Check the data attribute of our vector to make sure we got memory */
    if (retval->data == NULL) {
        free(retval); //Why is this line necessary? Ans: We have allocate memory for retval
        allocation_failed(__LINE__);
    }

    /* Complete the initialization by setting the single component to zero */
    retval->data[0] = 0;

    /* and return... */
    return retval; /* UPDATE RETURN VALUE */
}

/* Return the value at the specified location/component "loc" of the vector */
int vector_get(vector_t *v, size_t loc) {

    /* If we are passed a NULL pointer for our vector, complain about it and exit. */
    if(v == NULL) {
        fprintf(stderr, "vector_get: passed a NULL vector.\n");
        abort();
    }

    /* If the requested location is higher than we have allocated, return 0.
     * Otherwise, return what is in the passed location.
     */
    /* YOUR CODE HERE */
    if (loc > v->size - 1) {
        return 0;
    } else {
        return v->data[loc];
    }

}

/* Free up the memory allocated for the passed vector.
   Remember, you need to free up ALL the memory that was allocated. */
void vector_delete(vector_t *v) {
    /* YOUR CODE HERE */
    if (v) {
        if (v->data) {
            free(v->data);
            v->data = NULL;
        }
        free(v);
        v = NULL;
    }
}

/* Set a value in the vector. If the extra memory allocation fails, call
   allocation_failed(). */
void vector_set(vector_t *v, size_t loc, int value) {
    /* What do you need to do if the location is greater than the size we have
     * allocated?  Remember that unset locations should contain a value of 0.
     */

    /* YOUR CODE HERE */
    if (v == NULL) {
        fprintf(stderr, "Vector hasn't been allocated.\n");
        abort();
    }

    if (loc > v->size - 1) {
        size_t extra_len = loc - v->size + 1;

        v->data = (int *)realloc(v->data, sizeof(int) * (extra_len + v->size));

        if (v->data == NULL) {
            allocation_failed(__LINE__);
        }

        int *data_ptr = v->data + v->size;
        // for (int i = 0; i < extra_len - 1; i++) {
        //     data_ptr[i] = 0;
        // }
        memset(data_ptr, 0, sizeof(int) * extra_len);

        v->size = v->size + extra_len;
        v->data[v->size - 1] = value;
    } else {
        v->data[loc] = value;
    }
}