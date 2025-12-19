#pragma once

#include "file-io.h" 

// Узел дерева Хаффмана
typedef struct HuffmanNode {
    unsigned char symbol;      
    unsigned int freq;         
    struct HuffmanNode *left;  
    struct HuffmanNode *right; 
} HuffmanNode;

// Min-heap для узлов дерева
typedef struct {
    HuffmanNode **nodes;       
    int size;                  
    int capacity;              
} MinHeap;

// Функции для работы с узлами
HuffmanNode* create_node(unsigned char symbol, unsigned int freq);
HuffmanNode* create_internal_node(HuffmanNode *left, HuffmanNode *right);
void free_huffman_tree(HuffmanNode *root);

// Функции для работы с минимальной кучей
MinHeap* create_min_heap(int capacity);
void destroy_min_heap(MinHeap *heap);
void insert_min_heap(MinHeap *heap, HuffmanNode *node);
HuffmanNode* extract_min(MinHeap *heap);
void min_heapify(MinHeap *heap, int index);
void build_min_heap(MinHeap *heap);

// Основные функции алгоритма Хаффмана
HuffmanNode* build_huffman_tree(Pair *freq_table, int size);
void generate_huffman_codes(HuffmanNode *root, char **codes, char *buffer, int depth);
void print_huffman_codes(HuffmanNode *root);  

void encode_file(FILE *input, FILE *output, char **codes, int total_symbols);
unsigned long long get_encoded_size(char **codes, int *freq_array);
void decode_file(FILE *input, FILE *output, HuffmanNode *root);
