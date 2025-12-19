#include "file-io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Pair *build_frequency_table(FILE *input, int *table_size) {
    // Массив для частот символов
    unsigned int freq_array[256] = {0};
    unsigned char buffer[1024];
    int bytes_read;

    // Подсчет частот символов
    while ((bytes_read = fread(buffer, 1, 1024, input)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            freq_array[buffer[i]]++;
        }
    }

    // Подсчет количества уникальных символов
    int unique_count = 0;
    for (int i = 0; i < 256; i++) {
        if (freq_array[i] > 0) {
            unique_count++;
        }
    }

    // Создаем массив пар только для символов с freq > 0
    Pair *freq_table = (Pair*) calloc(unique_count, sizeof(Pair));
    if (!freq_table) {
        *table_size = 0;
        return NULL;
    }

    // Заполняем массив пар
    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (freq_array[i] > 0) {
            freq_table[index].ch = (unsigned char)i;
            freq_table[index].freq = freq_array[i];
            index++;
        }
    }

    // Сортируем массив пар (по частоте, затем по символу)
    for (int i = 0; i < unique_count - 1; i++) {
        for (int j = i + 1; j < unique_count; j++) {
            if (freq_table[j].freq < freq_table[i].freq) {
                Pair temp = freq_table[i];
                freq_table[i] = freq_table[j];
                freq_table[j] = temp;
            } else if (freq_table[j].freq == freq_table[i].freq && 
                       freq_table[j].ch < freq_table[i].ch) {
                Pair temp = freq_table[i];
                freq_table[i] = freq_table[j];
                freq_table[j] = temp;
            }
        }
    }

    // Возвращаем результат
    *table_size = unique_count;
    return freq_table;
}