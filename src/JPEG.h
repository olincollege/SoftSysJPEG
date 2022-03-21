#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <math.h>
#include <string.h>
#include "cvector.h"

typedef unsigned char byte;
#define ROUND_DIV(a,b) ((a<0) ^ (b<0)) ? (a - b/2)/b: (a + b/2)/b
#define MAT_SIZE 8
#define RESTART_INTERVAL 0

// #define SOI 0xD8 // Start of Image
// #define EOI 0xD9 // End of Image
// #define SOS 0xDA // Start of Scan
// #define DQT 0xDB // Define Quantization Table(s)
// #define DNL 0xDC // Define Number of Lines
// #define DRI 0xDD // Define Restart Interval
// #define DHP 0xDE // Define Hierarchical Progression
// #define EXP 0xDF // Expand Reference Component(s)

// TODO remove extra markers and change to use define
const byte SOF0 = 0xC0;  // Baseline DCT
const byte SOF1 = 0xC1;  // Extended sequential DCT
const byte SOF2 = 0xC2;  // Progressive DCT
const byte SOF3 = 0xC3;  // Lossless (sequential)

// Start of Frame markers, differential, Huffman coding
const byte SOF5 = 0xC5;  // Differential sequential DCT
const byte SOF6 = 0xC6;  // Differential progressive DCT
const byte SOF7 = 0xC7;  // Differential lossless (sequential)

// Start of Frame markers, non-differential, arithmetic coding
const byte SOF9 = 0xC9;   // Extended sequential DCT
const byte SOF10 = 0xCA;  // Progressive DCT
const byte SOF11 = 0xCB;  // Lossless (sequential)

// Start of Frame markers, differential, arithmetic coding
const byte SOF13 = 0xCD;  // Differential sequential DCT
const byte SOF14 = 0xCE;  // Differential progressive DCT
const byte SOF15 = 0xCF;  // Differential lossless (sequential)

// Define Huffman Table(s)
const byte DHT = 0xC4;

// JPEG extensions
const byte JPG = 0xC8;

// Define Arithmetic Coding Conditioning(s)
const byte DAC = 0xCC;

// Restart interval Markers
const byte RST0 = 0xD0;
const byte RST1 = 0xD1;
const byte RST2 = 0xD2;
const byte RST3 = 0xD3;
const byte RST4 = 0xD4;
const byte RST5 = 0xD5;
const byte RST6 = 0xD6;
const byte RST7 = 0xD7;

// Other Markers
const byte SOI = 0xD8;  // Start of Image
const byte EOI = 0xD9;  // End of Image
const byte SOS = 0xDA;  // Start of Scan
const byte DQT = 0xDB;  // Define Quantization Table(s)
const byte DNL = 0xDC;  // Define Number of Lines
const byte DRI = 0xDD;  // Define Restart Interval
const byte DHP = 0xDE;  // Define Hierarchical Progression
const byte EXP = 0xDF;  // Expand Reference Component(s)

// APPN Markers
const byte APP0 = 0xE0;
const byte APP1 = 0xE1;
const byte APP2 = 0xE2;
const byte APP3 = 0xE3;
const byte APP4 = 0xE4;
const byte APP5 = 0xE5;
const byte APP6 = 0xE6;
const byte APP7 = 0xE7;
const byte APP8 = 0xE8;
const byte APP9 = 0xE9;
const byte APP10 = 0xEA;
const byte APP11 = 0xEB;
const byte APP12 = 0xEC;
const byte APP13 = 0xED;
const byte APP14 = 0xEE;
const byte APP15 = 0xEF;

// Misc Markers
const byte JPG0 = 0xF0;
const byte JPG1 = 0xF1;
const byte JPG2 = 0xF2;
const byte JPG3 = 0xF3;
const byte JPG4 = 0xF4;
const byte JPG5 = 0xF5;
const byte JPG6 = 0xF6;
const byte JPG7 = 0xF7;
const byte JPG8 = 0xF8;
const byte JPG9 = 0xF9;
const byte JPG10 = 0xFA;
const byte JPG11 = 0xFB;
const byte JPG12 = 0xFC;
const byte JPG13 = 0xFD;
const byte COM = 0xFE;
const byte TEM = 0x01;

typedef unsigned int* QuantizationTable;

typedef struct table {
    unsigned char offsets[17];
    unsigned char symbols[176];
    unsigned char codes[176];
    
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

const byte zigzag[] = {0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18,
                       11, 4,  5,  12, 19, 26, 33, 40, 48, 41, 34, 27, 20,
                       13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43,
                       36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45,
                       38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};

unsigned int  yQuantTable[64] =  {        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1};
unsigned int  CrCbQuantTable[64] =  {        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1};
// unsigned int yQuantTable[64] = {
//     16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
//     14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
//     18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 11,  92,
//     49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};

// unsigned int CrCbQuantTable[64] = {
//     16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
//     14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
//     18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 11,  92,
//     49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};


HuffmanTable hDCTableY = {
    { 0, 0, 1, 6, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12 },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b },
    {}
};

HuffmanTable hDCTableCbCr = {
    { 0, 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12 },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b },
    {}
};

HuffmanTable hACTableY = {
    { 0, 0, 2, 3, 6, 9, 11, 15, 18, 23, 28, 32, 36, 36, 36, 37, 162 },
    {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
        0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    },
    {}
};

HuffmanTable hACTableCbCr = {
    { 0, 0, 2, 3, 5, 9, 13, 16, 20, 27, 32, 36, 40, 40, 41, 43, 162 },
    {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
        0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    },
    {}
};

HuffmanTable* const dcTables[] = { &hDCTableY, &hDCTableCbCr, &hDCTableCbCr };
HuffmanTable* const acTables[] = { &hACTableY, &hACTableCbCr, &hACTableCbCr };