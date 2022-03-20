#pragma once
#include <stdio.h>
typedef unsigned char byte;


// #define SOI 0xD8 // Start of Image
// #define EOI 0xD9 // End of Image
// #define SOS 0xDA // Start of Scan
// #define DQT 0xDB // Define Quantization Table(s)
// #define DNL 0xDC // Define Number of Lines
// #define DRI 0xDD // Define Restart Interval
// #define DHP 0xDE // Define Hierarchical Progression
// #define EXP 0xDF // Expand Reference Component(s)

const byte SOF0 = 0xC0; // Baseline DCT
const byte SOF1 = 0xC1; // Extended sequential DCT
const byte SOF2 = 0xC2; // Progressive DCT
const byte SOF3 = 0xC3; // Lossless (sequential)

// Start of Frame markers, differential, Huffman coding
const byte SOF5 = 0xC5; // Differential sequential DCT
const byte SOF6 = 0xC6; // Differential progressive DCT
const byte SOF7 = 0xC7; // Differential lossless (sequential)

// Start of Frame markers, non-differential, arithmetic coding
const byte SOF9 = 0xC9; // Extended sequential DCT
const byte SOF10 = 0xCA; // Progressive DCT
const byte SOF11 = 0xCB; // Lossless (sequential)

// Start of Frame markers, differential, arithmetic coding
const byte SOF13 = 0xCD; // Differential sequential DCT
const byte SOF14 = 0xCE; // Differential progressive DCT
const byte SOF15 = 0xCF; // Differential lossless (sequential)

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
const byte SOI = 0xD8; // Start of Image
const byte EOI = 0xD9; // End of Image
const byte SOS = 0xDA; // Start of Scan
const byte DQT = 0xDB; // Define Quantization Table(s)
const byte DNL = 0xDC; // Define Number of Lines
const byte DRI = 0xDD; // Define Restart Interval
const byte DHP = 0xDE; // Define Hierarchical Progression
const byte EXP = 0xDF; // Expand Reference Component(s)

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

typedef unsigned int * QuantizationTable;


typedef struct image
{
    int height;
    int width;        
} Image;

const byte zigzag[] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

unsigned int yQuantTable[64] = {
        16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
        14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
        18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 11,  92,
        49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};

unsigned int CrCbQuantTable[64] = {
        16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
        14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
        18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 11,  92,
        49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};