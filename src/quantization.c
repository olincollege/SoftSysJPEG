#include <stdlib.h>
#include <stdio.h>
#define rows 8
#define cols 8
#define ROUND_DIV(a,b) ((a<0) ^ (b<0)) ? (a - b/2)/b: (a + b/2)/b

void quantize(int* data, int* quantization_table, int len) {
    for (size_t i = 0; i < len; i++)
    {
        data[i] = ROUND_DIV(data[i], quantization_table[i]);
    }
    
}

int main(int argc, char const* argv[]) {
    int data[rows * cols] = {
        313, 56,  -27, 18,  78,  -60, 27, -27, -38, -27, 13, 44,  32,
        -1,  -24, -10, -20, -17, 10,  33, 21,  -6,  -16, -9, -10, -8,
        9,   17,  9,   -10, -13, 1,   -6, 1,   6,   4,   -3, -7,  -5,
        5,   2,   3,   0,   -3,  -7,  -4, 0,   3,   4,   4,  -1,  -2,
        -9,  0,   2,   4,   3,   1,   0,  -4,  -2,  -1,  3,  1};
    int quantization_data[rows * cols] = {
        16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
        14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
        18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 11,  92,
        49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
    quantize(data, quantization_data, rows*cols);
    for (size_t i = 0; i < 64; i++) {
        printf("%i,", data[i]);
    }
    return 0;
}
