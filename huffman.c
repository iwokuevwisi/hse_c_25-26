#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HuffmanNode* create_node(unsigned char symbol, unsigned int freq) {
    HuffmanNode *node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) {
        return NULL;
    }
    
    node->symbol = symbol;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

HuffmanNode* create_internal_node(HuffmanNode *left, HuffmanNode *right) {
    // Внутренний узел не содержит символа (устанавливаем 0)
    HuffmanNode *node = create_node(0, left->freq + right->freq);
    if (!node) {
        return NULL;
    }
    
    node->left = left;
    node->right = right;
    
    return node;
}

void free_huffman_tree(HuffmanNode *root) {
    if (!root) return;
    
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}

MinHeap* create_min_heap(int capacity) {
    MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) {
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    heap->nodes = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    
    if (!heap->nodes) {
        free(heap);
        return NULL;
    }
    
    return heap;
}

void destroy_min_heap(MinHeap *heap) {
    if (!heap) return;
    
    free(heap->nodes);
    free(heap);
}

// Функция для сравнения двух узлов
int compare_nodes(HuffmanNode *a, HuffmanNode *b) {
    if (a->freq != b->freq) {
        return a->freq - b->freq;
    }

    return a->symbol - b->symbol;
}

void min_heapify(MinHeap *heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    if (left < heap->size && 
        compare_nodes(heap->nodes[left], heap->nodes[smallest]) < 0) {
        smallest = left;
    }
    
    if (right < heap->size && 
        compare_nodes(heap->nodes[right], heap->nodes[smallest]) < 0) {
        smallest = right;
    }
    
    if (smallest != index) {
        HuffmanNode *temp = heap->nodes[index];
        heap->nodes[index] = heap->nodes[smallest];
        heap->nodes[smallest] = temp;
        
        min_heapify(heap, smallest);
    }
}

// Построение кучи 
void build_min_heap(MinHeap *heap) {
    for (int i = (heap->size - 2) / 2; i >= 0; i--) {
        min_heapify(heap, i);
    }
}

// Вставка нового узла в кучу
void insert_min_heap(MinHeap *heap, HuffmanNode *node) {
    if (heap->size >= heap->capacity) {
        heap->capacity *= 2;
        heap->nodes = (HuffmanNode**)realloc(heap->nodes, heap->capacity * sizeof(HuffmanNode*));
    }
    
    int index = heap->size;
    heap->nodes[index] = node;
    heap->size++;
    
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (compare_nodes(heap->nodes[index], heap->nodes[parent]) >= 0) {
            break;
        }
        
        HuffmanNode *temp = heap->nodes[index];
        heap->nodes[index] = heap->nodes[parent];
        heap->nodes[parent] = temp;
        
        index = parent;
    }
}

// Извлечение минимального элемента (корня)
HuffmanNode* extract_min(MinHeap *heap) {
    if (heap->size == 0) {
        return NULL;
    }
    
    HuffmanNode *min = heap->nodes[0];
    
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    
    min_heapify(heap, 0);
    
    return min;
}

// Построение самого дерева Хаффмана
HuffmanNode* build_huffman_tree(Pair *freq_table, int size) {
    if (size == 0) {
        return NULL;
    }
    
    // Создаем минимальную кучу
    MinHeap *heap = create_min_heap(size);
    if (!heap) {
        return NULL;
    }
    
    // Создаем листья для всех символов и добавляем в кучу
    for (int i = 0; i < size; i++) {
        HuffmanNode *leaf = create_node(freq_table[i].ch, freq_table[i].freq);
        if (!leaf) {
            destroy_min_heap(heap);
            return NULL;
        }
        insert_min_heap(heap, leaf);
    }
    
    // Построение дерева: объединяем два наименьших узла, пока не останется один
    while (heap->size > 1) {
        // Извлекаем два узла с минимальной частотой
        HuffmanNode *left = extract_min(heap);
        HuffmanNode *right = extract_min(heap);
        
        // Создаем новый внутренний узел
        HuffmanNode *parent = create_internal_node(left, right);
        if (!parent) {
            free_huffman_tree(left);
            free_huffman_tree(right);
            destroy_min_heap(heap);
            return NULL;
        }
        
        // Добавляем новый узел обратно в кучу
        insert_min_heap(heap, parent);
    }
    
    // Последний оставшийся узел - корень дерева
    HuffmanNode *root = extract_min(heap);
    
    // Очищаем кучу
    destroy_min_heap(heap);
    
    return root;
}

// Функция для генерации кодов Хаффмана
void generate_codes_recursive(HuffmanNode *root, char **codes, char *buffer, int depth) {
    if (!root) {
        return;
    }
    
    // Если это лист
    if (!root->left && !root->right) {
        buffer[depth] = '\0';
        // Копируем код в таблицу кодов
        codes[root->symbol] = strdup(buffer);
        return;
    }
    
    // Идем в левого потомка (бит 0)
    if (root->left) {
        buffer[depth] = '0';
        generate_codes_recursive(root->left, codes, buffer, depth + 1);
    }
    
    // Идем в правого потомка (бит 1)
    if (root->right) {
        buffer[depth] = '1';
        generate_codes_recursive(root->right, codes, buffer, depth + 1);
    }
}

// Генерация кодов Хаффмана для всех символов
void generate_huffman_codes(HuffmanNode *root, char **codes, char *buffer, int depth) {
    // Инициализируем таблицу кодов NULL
    for (int i = 0; i < 256; i++) {
        codes[i] = NULL;
    }
    
    // Если дерево из одного узла
    if (root && !root->left && !root->right) {
        codes[root->symbol] = strdup("0");
        return;
    }
    
    // Обычный случай
    generate_codes_recursive(root, codes, buffer, depth);
}

// Отладка - печать кодов
void print_huffman_codes(HuffmanNode *root) {
    char *codes[256];
    char buffer[256];
    
    generate_huffman_codes(root, codes, buffer, 0);
    
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
    
    // Очищаем память
    for (int i = 0; i < 256; i++) {
        free(codes[i]);
    }
}

// Функция для получения размера закодированных данных
unsigned long long get_encoded_size(char **codes, int *freq_array) {
    unsigned long long total_bits = 0;
    
    for (int i = 0; i < 256; i++) {
        if (codes[i] != NULL && freq_array[i] > 0) {
            total_bits += strlen(codes[i]) * freq_array[i];
        }
    }
    
    return total_bits;
}

void encode_file(FILE *input, FILE *output, char **codes, int total_symbols) {
    if (!input || !output || !codes) {
        return;
    }
    
    rewind(input);
    
    // Записываем количество символов В НАЧАЛО файла
    fwrite(&total_symbols, sizeof(int), 1, output);
    
    printf("Writing %d symbols to compressed file...\n", total_symbols);
    
    unsigned char buffer[1024];
    int bytes_read;
    unsigned char bit_buffer = 0;
    int bit_count = 0;
    int symbols_processed = 0;
    
    // Кодируем данные
    while ((bytes_read = fread(buffer, 1, 1024, input)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            unsigned char symbol = buffer[i];
            symbols_processed++;
            
            char *code = codes[symbol];
            if (!code) {
                printf("Warning: No code for symbol %d\n", symbol);
                continue;
            }
            
            // Записываем биты кода
            for (int j = 0; code[j] != '\0'; j++) {
                if (code[j] == '1') {
                    bit_buffer |= (1 << (7 - bit_count));
                }
                
                bit_count++;
                
                if (bit_count == 8) {
                    fwrite(&bit_buffer, 1, 1, output);
                    bit_buffer = 0;
                    bit_count = 0;
                }
            }
        }
    }
    
    // Записываем последний неполный байт
    if (bit_count > 0) {
        fwrite(&bit_buffer, 1, 1, output);
    }
    
    printf("Encoded %d symbols (expected: %d)\n", symbols_processed, total_symbols);
}

void decode_file(FILE *input, FILE *output, HuffmanNode *root) {
    // Читаем количество символов из начала файла
    int total_symbols;
    if (fread(&total_symbols, sizeof(int), 1, input) != 1) {
        printf("Error: Cannot read total symbols from file\n");
        return;
    }
    
    printf("Decoding %d symbols...\n", total_symbols);

    // Если дерево из одного узла
    if (!root->left && !root->right) {
        printf("Special case: single character tree\n");
        for (int i = 0; i < total_symbols; i++) {
            fwrite(&root->symbol, 1, 1, output);
        }
        printf("Successfully decoded %d symbols (single character)\n", total_symbols);
        return;
    }
    
    // Декодируем бит за битом
    unsigned char byte;
    HuffmanNode *current = root;
    int symbols_decoded = 0;
    
    while (symbols_decoded < total_symbols) {
        if (fread(&byte, 1, 1, input) != 1) {
            printf("Error: Unexpected end of file\n");
            break;
        }
        
        // Обрабатываем каждый бит в байте (старший бит первый)
        for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
            int bit = (byte >> bit_pos) & 1;
            
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            // Проверяем, что узел существует
            if (!current) {
                printf("Error: Invalid Huffman code\n");
                return;
            }
            
            // Если достигли листа
            if (!current->left && !current->right) {
                fwrite(&current->symbol, 1, 1, output);
                symbols_decoded++;
                // Тут возвращаемся к корню
                current = root; 
                
                // Если декодировали все символы, выходим
                if (symbols_decoded >= total_symbols) {
                    printf("Successfully decoded %d symbols\n", symbols_decoded);
                    return;
                }
            }
        }
    }
    
    if (symbols_decoded != total_symbols) {
        printf("Warning: Decoded %d symbols, expected %d\n", 
               symbols_decoded, total_symbols);
    }
}
