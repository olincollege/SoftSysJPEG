#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <math.h>
#include <string.h>
#include "cvector.h"
#include "cbmp.h"
#include <stdint.h>

typedef unsigned int uint;
typedef unsigned char byte;
#define ROUND_DIV(a,b) ((a<0) ^ (b<0)) ? (a - b/2)/b: (a + b/2)/b
#define MAT_SIZE 8
#define RESTART_INTERVAL 0

typedef unsigned int* QuantizationTable;

typedef struct table {
    unsigned char offsets[17];
    unsigned char symbols[176];
    uint codes[176];
    
} HuffmanTable;

typedef struct block {
    int y[64];
    int r[64];
    int cb[64];
    int g[64];
    int cr[64];
    int b[64];
} Block;

typedef struct image {
    int height;
    int width;
    Block* blocks;
    int blockHeight;
    int blockWidth;

} Image;


int read_bmp(BMP * bmp, Image * image);
void rgb_to_ycrcb(Image *image);
void chroma_downsample(int* cb, int* cr, int height, int width);
void slice_to_mat(int* arr, gsl_matrix *mat);
void mat_to_slice(int* arr, gsl_matrix *mat);
void dct_slice(int* slice);