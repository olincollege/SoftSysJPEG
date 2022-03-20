#include "JPEG.h"

void write_short(unsigned int val, FILE * file) {
    // write a 16 bit number in big endian 
    fputc((val >> 8) & 0xFF, file); // shift to select left half and and with FF to select 1 byte
    fputc((val >> 0) & 0xFF, file); // same as above but select right half
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
    // write default header 
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

int read_bmp(char * file_path, Image * image) {
    unsigned char *data = 0;
    unsigned char info[54] = {0};
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return 0;
    }
    fread(info, 1, 54, file);
    image->width = *(int *)(info + 18);
    image->height = *(int *)(info + 22);
    int bitcount = *(int *)(info + 28);
    int size = (((image->width) * (bitcount) + 31) / 32) * 4 * (image->height);
    int padding = (image->width) % 4;
    if (bitcount != 24) {  // this code works for 24-bit bitmap only
        if (file) fclose(file);
        if (data) free(data);
        return 0;
    }

    data = malloc((size));
    fread(data, 1, (size), file);
    image->blockHeight = (image->height + 7) / 8; // TODO figure out why theres a plus 7 here
    image->blockWidth = (image->width + 7) / 8;
    printf("size | w:%i h:%i\n", image->width, image->height);
    printf("blocks | w:%i h:%i \n", image->blockHeight, image->blockWidth);
    image->blocks = malloc(sizeof(Block) * (image->blockHeight * image->blockWidth));

    for (int row = image->height - 1; row >= 0; row--) {
        const int blockRow = row / 8;
        const int pixelRow = row % 8;
        for (int col = 0; col < image->width; col++) {
            const int blockColumn = col / 8;
            const int pixelColumn = col % 8;
            const int blockIndex = blockRow * image->blockWidth + blockColumn;
            const int pixelIndex = pixelRow * 8 + pixelColumn;

            int p = (row * image->width + col) * 3 + row * padding;
            image->blocks[blockIndex].r[pixelIndex] = data[p + 0];
            image->blocks[blockIndex].g[pixelIndex] = data[p + 1];
            image->blocks[blockIndex].b[pixelIndex] = data[p + 2];
            // *((red+row*width) + col) = data[p + 0];
            // *((green+row*width) + col) = data[p + 1];
            // *((blue+row*width) + col) = data[p + 2];
            // red[row][col] = data[p + 0];
            // green[row][col] = data[p + 1];
            // blue[row][col] = data[p + 2];
        }
    }
    return 0;
    
}

void rgb_to_ycrcb(Image * image){
    int r,g,b;
    for (int i = 0; i < image->blockHeight*image->blockWidth; i++){
        for (int j = 0; j < 64; j++){
            r = image->blocks[i].r[j];
            g = image->blocks[i].g[j];
            b = image->blocks[i].b[j];
            image->blocks[i].y[j] = 16+ (((r<<6)+(r<<1)+(g<<7)+g+(b<<4)+(b<<3)+b)>>8);
            image->blocks[i].cb[j] = 128 + ((-((r<<5)+(r<<2)+(r<<1))-((g<<6)+(g<<3)+(g<<1))+(b<<7)-(b<<1))>>8);
            image->blocks[i].cr[j] = 128 + (((r<<7)-(r<<4)-((g<<6)+(g<<5)-(g<<1))-((b<<4)+(b<<1)))>>8); 
        }
    }
}



int main(int argc, char const *argv[])
{
    // read bmp
    // convert to YCrCb
    // subsample
    // DCT
    // Quantize
    // huffman
    
    Image * image;    // for (size_t i = 0; i < image->blockHeight*image->blockWidth; i++)
    // {
    //     printf("Block # %i\n", i);
    //     for (size_t j = 0; j < 64; j++)
    //     {
    //         printf("%i %i %i\n", image->blocks[i].r[j], image->blocks[i].g[j], image->blocks[i].b[j]);
    //     }        
    // }

    // read in the bmp TODO figure out why only 24 bit images work
    if (read_bmp("include/data/test.bmp", image)) {
        printf("Error reading BMP");
        exit(-1);
    }
    // for (size_t i = 0; i < image->blockHeight*image->blockWidth; i++)
    // {
    //     printf("Block # %i\n", i);
    //     for (size_t j = 0; j < 64; j++)
    //     {
    //         printf("%i %i %i\n", image->blocks[i].r[j], image->blocks[i].g[j], image->blocks[i].b[j]);
    //     }        
    // }

    // Convert to ycrcb
    rgb_to_ycrcb(image);

    for (size_t i = 0; i < image->blockHeight*image->blockWidth; i++)
    {
        printf("Block # %i\n", i);
        for (size_t j = 0; j < 64; j++)
        {
            printf("rgb %i %i %i\n", image->blocks[i].r[j], image->blocks[i].g[j], image->blocks[i].b[j]);
            printf("ycrcb %i %i %i\n", image->blocks[i].y[j], image->blocks[i].cr[j], image->blocks[i].cb[j]);
        }        
    }

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
