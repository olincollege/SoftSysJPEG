#include "JPEG.h"

void write_short(unsigned int val, FILE * file) {
    fputc((val >> 8) & 0xFF, file);
    fputc((val >> 0) & 0xFF, file);
}

void write_quantization_table(FILE * file, QuantizationTable table, unsigned int tableID) {
    fputc(0xFF, file);
    fputc(DQT, file);
    write_short(67, file); // number of bytes including this byte
    fputc(tableID, file);
    for (size_t i = 0; i < 64; i++)
    {
        fputc(table[zigzag[i]], file);
    }
}

void write_app_header(FILE * file) {
    fputc(0xFF, file);
    fputc(APP0, file);
    write_short(16, file);
    fputc('J', file);
    fputc('F', file);
    fputc('I', file);
    fputc('F', file);
    fputc(0, file);
    fputc(1, file);
    fputc(2, file);
    fputc(0, file);
    write_short(100, file);
    write_short(100, file);
    fputc(0, file);
    fputc(0, file);
}

void write_start_of_frame(FILE * file, Image * image) {
    fputc(0xFF, file);
    fputc(SOF0, file);
    write_short(17, file); // size of block
    fputc(8, file); // precision must be 8 lol
    write_short(image->height, file);
    write_short(image->width, file);
    fputc(3, file); // num of components (1 for grayscale and 3 for ycrcb)
    // y
    fputc(1, file);
    fputc(0x11, file); // sampling factor of 1
    fputc(0, file); // assign y to first quant table
    // cr
    fputc(2, file);
    fputc(0x11, file); // sampling factor of 1
    fputc(0, file); // assign cr to first quant table
    // cb
    fputc(3, file);
    fputc(0x11, file); // sampling factor of 1
    fputc(0, file); // assign cb to first quant table
}

int main(int argc, char const *argv[])
{
    // read bmp
    // convert to YCrCb
    // subsample
    // DCT
    // Quantize
    // huffman
    
    Image * image;
    image->height = 64;
    image->width = 64;


    FILE *file;

    file = fopen("include/test.jpg", "w+");

    // Start of Image
    fputc(0xFF, file);
    fputc(SOI, file);

    // APP0 i dont really understand this one
    write_app_header(file);

    // Quantization Tables
    write_quantization_table(file, yQuantTable, 0);
    write_quantization_table(file, CrCbQuantTable, 1);


    // Start of Frame
    write_start_of_frame(file, image);
    // Huffman

    // Start of Scan

    // Huffman data

    // End of Image
    fputc(0xFF, file);
    fputc(EOI, file);

    fclose(file);
    return 0;
}
