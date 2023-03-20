#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ex1.h"

/* Returns the number of times LETTER appears in STR.
There are two different ways to iterate through a string.
1st way hint: strlen() may be useful
2nd way hint: all strings end in a null terminator */
int num_occurrences(char *str, char letter) {
    /* TODO: implement num_occurances */
    size_t str_len = strlen((const char *)str);
    size_t count = 0;

    for (int idx = 0; idx < str_len; idx++) {
        if (*(str + idx) == letter) {
            count++;
        }
    }
    return count;
}

/* Populates DNA_SEQ with the number of times each nucleotide appears.
Each sequence will end with a NULL terminator and will have up to 20 nucleotides.
All letters will be upper case. */
void compute_nucleotide_occurrences(DNA_sequence *dna_seq) {
    /* TODO: implement compute_nucleotide_occurances */
    char *dna_seq_ptr = dna_seq->sequence;
    dna_seq->A_count = 0;
    dna_seq->C_count = 0;
    dna_seq->G_count = 0;
    dna_seq->T_count = 0;

    while (*dna_seq_ptr != '\0') {
        switch (*dna_seq_ptr) {
        case 'A': dna_seq->A_count++; dna_seq_ptr++; break;
        case 'C': dna_seq->C_count++; dna_seq_ptr++; break;
        case 'G': dna_seq->G_count++; dna_seq_ptr++; break;
        case 'T': dna_seq->T_count++; dna_seq_ptr++; break;
        default:
            printf("Not a nucleotide\n");
            exit(1);
        }
    }
    return;
}
