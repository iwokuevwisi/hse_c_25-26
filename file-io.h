#pragma once

#include <stdio.h>

typedef struct {
    unsigned char ch;
    unsigned int freq;
} Pair;

Pair *build_frequency_table(FILE *input, int *table_size);