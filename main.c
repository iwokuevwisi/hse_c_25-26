#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file-io.h"
#include "huffman.h"

void print_freq_table(Pair* table, int size) {
    printf("Frequency table (%d unique symbols):\n", size);
    for (int i = 0; i < size; i++) {
        if (table[i].ch >= 32 && table[i].ch <= 126) {
            printf("  '%c' (ASCII %d): %d times\n", 
                   table[i].ch, table[i].ch, table[i].freq);
        } else {
            printf("  ASCII %d: %d times\n", 
                   table[i].ch, table[i].ch, table[i].freq);
        }
    }
}

int main(int argc, char* argv[]) {
    // Проверка аргументов
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_compressed_file>\n", argv[0]);
        printf("Example: %s input.txt compressed.bin\n", argv[0]);
        return 1;
    }

    char* input_filename = argv[1];
    char* output_filename = argv[2];
    
    // Открываем файлы
    FILE *input = fopen(input_filename, "rb");
    if (!input) {
        printf("Error: Cannot open input file '%s'\n", input_filename);
        return 1;
    }

    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        printf("Error: Cannot open output file '%s'\n", output_filename);
        fclose(input);
        return 1;
    }

    printf("Processing file: %s\n", input_filename);
    
    // Строим таблицу частот
    int table_size;
    Pair *freq_table = build_frequency_table(input, &table_size);
    
    if (!freq_table || table_size == 0) {
        printf("Error: Empty input file or memory allocation failed\n");
        fclose(input);
        fclose(output);
        return 1;
    }
    
    // Выводим таблицу частот
    print_freq_table(freq_table, table_size);
    
    // Подсчитываем общее количество символов
    int total_symbols = 0;
    for (int i = 0; i < table_size; i++) {
        total_symbols += freq_table[i].freq;
    }
    printf("\nTotal symbols in file: %d\n", total_symbols);
    
    // Строим дерево Хаффмана
    HuffmanNode *huffman_tree = build_huffman_tree(freq_table, table_size);
    
    if (!huffman_tree) {
        printf("Error: Failed to build Huffman tree\n");
        free(freq_table);
        fclose(input);
        fclose(output);
        return 1;
    }
    
    printf("Huffman tree built successfully!\n");
    
    // Генерируем коды
    char *codes[256];
    char code_buffer[256];
    generate_huffman_codes(huffman_tree, codes, code_buffer, 0);
    
    // Вывод кодов
    printf("\nHuffman codes:\n");
    for (int i = 0; i < 256; i++) {
        if (codes[i] != NULL) {
            if (i >= 32 && i <= 126) {
                printf("  '%c' (ASCII %3d): %s\n", (char)i, i, codes[i]);
            } else {
                printf("  ASCII %3d: %s\n", i, codes[i]);
            }
        }
    }
    
    // Кодируем файл
    printf("\nEncoding file...\n");
    encode_file(input, output, codes, total_symbols);
    
    // Декодируем обратно для проверки
    printf("\n=== Testing decode ===\n");
    
    // Закрываем файлы для теста
    fclose(input);
    fclose(output);
    
    // Открываем сжатый файл для чтения
    FILE *compressed = fopen(output_filename, "rb");
    if (!compressed) {
        printf("Error: Cannot open compressed file for testing\n");
    } else {
        // Создаем файл для декодированных данных
        char decoded_filename[256];
        snprintf(decoded_filename, sizeof(decoded_filename), 
                "decoded_%s", input_filename);
        
        FILE *decoded = fopen(decoded_filename, "wb");
        if (!decoded) {
            printf("Error: Cannot create decoded file\n");
            fclose(compressed);
        } else {
            // Декодируем файл
            decode_file(compressed, decoded, huffman_tree);
            
            printf("Decoded file saved as: %s\n", decoded_filename);
            
            fclose(decoded);
            fclose(compressed);
            
            // Проверка корректности восстановления
            printf("\n=== Verification ===\n");
            
            // Открываем оригинальный и декодированный файлы для сравнения
            FILE *original = fopen(input_filename, "rb");
            FILE *decoded_check = fopen(decoded_filename, "rb");
            
            if (!original || !decoded_check) {
                printf("Cannot open files for verification\n");
            } else {
                int match = 1;
                int byte_pos = 0;
                int ch1, ch2;
                
                while (1) {
                    ch1 = fgetc(original);
                    ch2 = fgetc(decoded_check);
                    
                    // Оба файла закончились
                    if (ch1 == EOF && ch2 == EOF) {
                        break;  
                    }
                    
                    if (ch1 != ch2) {
                        printf("Mismatch at byte %d: original=%d, decoded=%d\n", 
                               byte_pos, ch1, ch2);
                        match = 0;
                        break;
                    }
                    
                    byte_pos++;
                }
                
                if (match) {
                    printf("SUCCESS: Original and decoded files are identical!\n");
                } else {
                    printf("FAIL: Files are different\n");
                }
                
                fclose(original);
                fclose(decoded_check);
            }
        }
    }
    
    // Очищаем память
    for (int i = 0; i < 256; i++) {
        free(codes[i]);
    }
    free_huffman_tree(huffman_tree);
    free(freq_table);

    return 0;
}
