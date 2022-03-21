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
    fputc(1, file); // assign cr to first quant table
    // cb
    fputc(3, file);
    fputc(0x11, file); // sampling factor of 1
    fputc(1, file); // assign cb to first quant table
}

int read_bmp(BMP * bmp, Image * image) {
    image->width = get_width(bmp);
    image->height = get_height(bmp);

    image->blockHeight = (image->height + 7) / 8; // TODO figure out why theres a plus 7 here
    image->blockWidth = (image->width + 7) / 8;
    printf("size | w:%i h:%i\n", image->width, image->height);
    printf("blocks | w:%i h:%i \n", image->blockHeight, image->blockWidth);
    image->blocks = malloc(sizeof(Block) * (image->blockHeight * image->blockWidth));
    unsigned char r, g, b;
    int k =0;
    for (size_t i = image->height-1; i < image->height; i--)
    {
        const int blockRow = k / 8;
        const int pixelRow = k % 8;
        for (size_t j = 0; j < image->width; j++)
        {
            get_pixel_rgb(bmp, j, i, &r, &g, &b);


            const int blockColumn = j / 8;
            const int pixelColumn = j % 8;
            const int blockIndex = blockRow * image->blockWidth + blockColumn;
            const int pixelIndex = pixelRow * 8 + pixelColumn;

            image->blocks[blockIndex].r[pixelIndex] = r; // BGR instead of rgb, idk why
            image->blocks[blockIndex].g[pixelIndex] = g; 
            image->blocks[blockIndex].b[pixelIndex] = b;
        }
        k++;
        
    }
    


    // for (int row = image->height - 1; row >= 0; row--) {
    //     const int blockRow = row / 8;
    //     const int pixelRow = row % 8;
    //     for (int col = 0; col < image->width; col++) {
    //         const int blockColumn = col / 8;
    //         const int pixelColumn = col % 8;
    //         const int blockIndex = blockRow * image->blockWidth + blockColumn;
    //         const int pixelIndex = pixelRow * 8 + pixelColumn;

    //         int p = (row * image->width + col) * 3 + row * padding;
    //         image->blocks[blockIndex].r[pixelIndex] = data[p + 0]; // BGR instead of rgb, idk why
    //         image->blocks[blockIndex].g[pixelIndex] = data[p + 1]; 
    //         image->blocks[blockIndex].b[pixelIndex] = data[p + 2];
    //     }
    // }
    return 0;
    
}

void rgb_to_ycrcb(Image * image){
    int r,g,b;
    for (int i = 0; i < image->blockHeight*image->blockWidth; i++){
        for (int j = 0; j < 64; j++){
            r = image->blocks[i].r[j];
            g = image->blocks[i].g[j];
            b = image->blocks[i].b[j];
            image->blocks[i].y[j] = round(16.0 + (65.738/256.0)*(float)r + (129.057/256.0)*(float)g + (25.064/256.0)*(float)b);
            image->blocks[i].cb[j] = round(128.0 + (-37.945/256.0)*(float)r + (-74.494/256.0)*(float)g + (112.439/256.0)*(float)b);
            image->blocks[i].cr[j] = round(128.0 + (112.439/256.0)*(float)r + (-94.154/256.0)*(float)g + (-18.285/256.0)*(float)b);
            // image->blocks[i].y[j] = 16 + (((r<<6)+(r<<1)+(g<<7)+g+(b<<4)+(b<<3)+b)>>8); // TODO comments explaining this
            // image->blocks[i].cb[j] = 128 + ((-((r<<5)+(r<<2)+(r<<1))-((g<<6)+(g<<3)+(g<<1))+(b<<7)-(b<<1))>>8);
            // image->blocks[i].cr[j] = 128 + (((r<<7)-(r<<4)-((g<<6)+(g<<5)-(g<<1))-((b<<4)+(b<<1)))>>8); 
        }
    }
}
void slice_to_mat(int* arr, gsl_matrix *mat){
    for (int i = 0; i < 64; i++){
        gsl_matrix_set(mat, i%8, i/8, arr[i]-128);
    }
}

void mat_to_slice(int* arr, gsl_matrix *mat){
    // printf("\n");
    for (int i = 0; i < 64; i++){
        // if (i%8 == 0) printf("\n");
        arr[i] = round(gsl_matrix_get(mat, i%8, i/8));
        // printf("%i\t", arr[i]);
    }
}

// TODO clean this up 
void dct_slice(int* slice){
    const float scale = sqrt(0.25);
    const float pi = 3.14159265;
    const float row_1 = sqrt(0.125);
    // Allocate matrices
    gsl_matrix *t = gsl_matrix_alloc(MAT_SIZE, MAT_SIZE);
    gsl_matrix *m = gsl_matrix_alloc(MAT_SIZE, MAT_SIZE);
    gsl_matrix *res = gsl_matrix_alloc(MAT_SIZE, MAT_SIZE);
    // Write the int array to the matrix
    slice_to_mat(slice, m);

    // Write the T matrix
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (i == 0){
                gsl_matrix_set(t, i, j, row_1);
            } else{
                gsl_matrix_set(t, i, j, cos(((2*j+1)*i*pi)/16)*scale);
            }
        }
    }

    //test out that this matrix printed correctly
    //multiply into DCT math
    gsl_matrix *inter = gsl_matrix_alloc(MAT_SIZE, MAT_SIZE);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, t, m, 0.0, inter);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, inter, t, 0.0, res);
    
    // free matrices declared in this function
    gsl_matrix_free(t);
    gsl_matrix_free(m);
    gsl_matrix_free(inter);

    // Save to to inputted slice
    mat_to_slice(slice, res);
}

void dct(Image * image) {
    for (size_t i = 0; i < image->blockHeight * image->blockWidth; i++) {
        dct_slice(image->blocks[i].y);
        dct_slice(image->blocks[i].cr);
        dct_slice(image->blocks[i].cb);        
    }
}

void quantize(Image *image, QuantizationTable yQuantTbl, QuantizationTable crcbQuantTbl) {
    for (size_t i = 0; i < image->blockHeight * image->blockWidth; i++) {
        for (size_t j = 0; j < 64; j++) {
            image->blocks[i].y[j] = ROUND_DIV(image->blocks[i].y[j], (signed)yQuantTbl[j]);
            image->blocks[i].cr[j] = ROUND_DIV(image->blocks[i].cr[j], (signed)crcbQuantTbl[j]);
            image->blocks[i].cb[j] = ROUND_DIV(image->blocks[i].cb[j], (signed)crcbQuantTbl[j]);
        }
    }
}

void restart_interval(Image * image, int interval) {
    // TODO
}

void write_huffman_table(FILE * file, unsigned char ac_or_dc, unsigned char tableID, HuffmanTable * table) {
    fputc(0xFF, file);
    fputc(DHT, file);
    write_short(19 + table->offsets[16], file);
    fputc(ac_or_dc << 4 | tableID, file);
    for (size_t i = 0; i < 16; i++)
    {
        fputc(table->offsets[i+1] - table->offsets[i], file);
    }
    for (size_t i = 0; i < 16; i++)
    {
        for (size_t j = table->offsets[i]; j < table->offsets[i + 1]; ++j) {
            fputc(table->symbols[j], file);
        }
        
    }
}

void write_start_of_scan(FILE * file) {
    fputc(0xFF, file);
    fputc(SOS, file);
    write_short(12, file);
    fputc(3, file);

    fputc(1, file);
    fputc(0x00, file);

    fputc(2, file);
    fputc(0x11, file);

    fputc(3, file);
    fputc(0x11, file);

    fputc(0, file);
    fputc(63, file);
    fputc(0, file);
}

void generateCodes(HuffmanTable *hTable) {
    uint code = 0;
    for (uint i = 0; i < 16; ++i) {
        for (uint j = hTable->offsets[i]; j < hTable->offsets[i + 1]; ++j) {
            hTable->codes[j] = code;
            code += 1;
        }
        code <<= 1;
    }
}

void getCode(const HuffmanTable* hTable, byte symbol, uint* code, uint* codeLength) {
    for (uint i = 0; i < 16; ++i) {
        for (uint j = hTable->offsets[i]; j < hTable->offsets[i + 1]; ++j) {
            if (symbol == hTable->symbols[j]) {
                (*code) = hTable->codes[j];
                (*codeLength) = i + 1;
                return;
            }
        }
    }
}

unsigned char nextBit = 0;
void writeBit(uint bit, unsigned char** data) {
    if (nextBit == 0) {
        cvector_push_back((*data), 0);
        // (*len)++;
        // data = realloc(data, sizeof(uint)*(*len));
        // data[(*len)-1] = 0;
    }
    (*data)[cvector_size((*data))-1] |= (bit & 1) << (7 - nextBit);
    nextBit = (nextBit + 1) % 8;
    if (nextBit == 0 && (*data)[cvector_size((*data))-1] == 0xFF) {
        cvector_push_back((*data), 0);
        // (*len)++;
        // data = realloc(data, sizeof(uint)*(*len));
        // data[(*len)-1] = 0;
    }
}

void writeBits(uint bits, uint length, unsigned char** data) {
    for (uint i = 1; i <= length; ++i) {
        writeBit(bits >> (length - i), data);
    }
}

uint bitLength(int v) {
    uint length = 0;
    while (v > 0) {
        v >>= 1;
        length += 1;
    }
    return length;
}

void encodeBlockComponent(
    unsigned char** data,
    int* const component,
    int* previousDC,
    HuffmanTable* dcTable,
    HuffmanTable* acTable
) {
    // encode DC value
    int coeff = component[0] - *previousDC;
    *previousDC = component[0];

    uint coeffLength = bitLength(abs(coeff));

    if (coeff < 0) {
        coeff += (1 << coeffLength) - 1;
    }

    uint code = 0;
    uint codeLength = 0;
    getCode(dcTable, coeffLength, &code, &codeLength);
    writeBits(code, codeLength, data);
    writeBits(coeff, coeffLength, data);

    // encode AC values
    for (uint i = 1; i < 64; ++i) {
        // find zero run length
        byte numZeroes = 0;
        while (i < 64 && component[zigzag[i]] == 0) {
            numZeroes += 1;
            i += 1;
        }

        if (i == 64) {
            getCode(acTable, 0x00, &code, &codeLength);
            writeBits(code, codeLength, data);
        }

        while (numZeroes >= 16) {
            getCode(acTable, 0xF0, &code, &codeLength);
            writeBits(code, codeLength, data);
            numZeroes -= 16;
        }

        // find coeff length
        coeff = component[zigzag[i]];
        coeffLength = bitLength(abs(coeff));

        if (coeff < 0) {
            coeff += (1 << coeffLength) - 1;
        }

        // find symbol in table
        byte symbol = numZeroes << 4 | coeffLength;
        getCode(acTable, symbol, &code, &codeLength);
        writeBits(code, codeLength, data);
        writeBits(coeff, coeffLength, data);
    }
}

void encode_huffman(Image * image, unsigned char ** huffmanData) {

    int previousDCs[3] = { 0 };

    for (uint i = 0; i < 2; ++i) {
        // if (!dcTables[i]->set) {
            generateCodes(dcTables[i]);
        // }
        // if (!acTables[i]->set) {
            generateCodes(acTables[i]);
        // }
    }

    for (uint y = 0; y < image->blockHeight; ++y) {
        for (uint x = 0; x < image->blockWidth; ++x) {

            encodeBlockComponent(
                        huffmanData,
                        image->blocks[y * image->blockWidth + x].y,
                        &previousDCs[0],
                        dcTables[0],
                        acTables[0]);
            encodeBlockComponent(
                        huffmanData,
                        image->blocks[y * image->blockWidth + x].cr,
                        &previousDCs[1],
                        dcTables[1],
                        acTables[1]);
            encodeBlockComponent(
                        huffmanData,
                        image->blocks[y * image->blockWidth + x].cb,
                        &previousDCs[2],
                        dcTables[2],
                        acTables[2]);
            
        }
    }

}

void print_blocks(Image * image) {
    for (size_t i = 0; i < image->blockHeight*image->blockWidth; i++)
    {
        // printf("\nBlock # %i\n", (int) i);
        for (size_t j = 0; j < 64; j++)
        {
            // if (j%8 == 0) printf("\n");
            // printf("rgb %i %i %i\n", image->blocks[i].r[j], image->blocks[i].g[j], image->blocks[i].b[j]);
            // printf("%i %i %i", image->blocks[i].y[j], image->blocks[i].cr[j], image->blocks[i].cb[j]);
            printf("%i,", image->blocks[i].cb[j]);
        }      
        printf("\n");  
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
    // int tmp[64] = {140,144,147,140,140,155,179,175,144,152,140,147,140,148,167,179,152,155,136,167,163,162,152,172,168,145,156,160,152,155,136,160,162,148,156,148,140,136,147,162,147,167,140,155,155,140,136,162,136,156,123,167,162,144,140,147,148,155,136,155,152,147,147,136};
    // dct_slice(tmp);
    // for (int j = 0; j < 64; j++){
    //     if (j%8 == 0) printf("\n");
    //     printf("%i\t", tmp[j]);
    //     }
    //     return 0;
    // int r = 150;
    // int g = 123;
    // int b = 153;
    // int y = round(16.0 + (65.738/256.0)*(float)r + (129.057/256.0)*(float)g + (25.064/256.0)*(float)b);
    // int cb = round(128.0 + (-37.945/256.0)*(float)r + (-74.494/256.0)*(float)g + (112.439/256.0)*(float)b);
    // int cr = round(128.0 + (112.439/256.0)*(float)r + (-94.154/256.0)*(float)g + (-18.285/256.0)*(float)b);
    // // int y = 16 + (((r<<6)+(r<<1)+(g<<7)+g+(b<<4)+(b<<3)+b)>>8); // TODO comments explaining this
    // // int cb = 128 + ((-((r<<5)+(r<<2)+(r<<1))-((g<<6)+(g<<3)+(g<<1))+(b<<7)-(b<<1))>>8);
    // // int cr = 128 + (((r<<7)-(r<<4)-((g<<6)+(g<<5)-(g<<1))-((b<<4)+(b<<1)))>>8); 
    // printf("%i %i %i\n", r,g,b);
    // printf("%i %i %i\n", y,cb,cr);
    // return 0;
    Image * image = malloc(sizeof(Image));
    BMP* bmp = bopen("include/data/smalldsf.bmp");
    // bmp = fopen("include/data/smalldsf.bmp", "r+");
    // // read in the bmp TODO figure out why only 24 bit images work
    if (read_bmp(bmp, image)) {
        printf("Error reading BMP\n");
        return EXIT_FAILURE;
    }
    bclose(bmp);
    // // Process image data to jpeg

    rgb_to_ycrcb(image);

    // // printf("YCRCB");
    // // print_blocks(image);
    dct(image);
    // // printf("DCT");
    // // print_blocks(image);
    quantize(image, yQuantTable, CrCbQuantTable);
    // printf("Quantize");
    print_blocks(image);
    // return 0;
    // restart_interval(image, RESTART_INTERVAL);
    // Write image data to file
    cvector_vector_type(unsigned char) huffman_data_vec = NULL;
    encode_huffman(image, &huffman_data_vec);
    FILE *file;

    file = fopen("include/test.jpg", "w+");

    // Start of Image
    fputc(0xFF, file);
    fputc(SOI, file);

    // Default application header
    write_app_header(file);

    // Quantization Tables
    write_quantization_table(file, yQuantTable, 0);
    write_quantization_table(file, CrCbQuantTable, 1);

    // Restart Interval
    // fputc(0xFF, file);
    // fputc(DRI, file);
    // write_short(RESTART_INTERVAL, file);

    // Start of Frame
    write_start_of_frame(file, image);
    
    // Huffman tables
    write_huffman_table(file, 0, 0, dcTables[0]);//&hDCTableY);
    write_huffman_table(file, 0, 1, dcTables[1]);//&hDCTableCbCr);
    write_huffman_table(file, 1, 0, acTables[0]);//&hACTableY);
    write_huffman_table(file, 1, 1, acTables[1]);//&hACTableCbCr);

    printf("----------------Offsets----------------\n");
    for (size_t i = 0; i < 17; i++)
    {
        printf("%i, ", dcTables[0]->offsets[i]);
    }
    printf("\n----------------Symbols----------------\n");
    for (size_t i = 0; i < 176; i++)
    {
        printf("%i, ", dcTables[0]->symbols[i]);
    }
    printf("\n----------------Codes----------------\n");
    for (size_t i = 0; i < 176; i++)
    {
        printf("%i, ", dcTables[0]->codes[i]);
    }
    
    // Table info 1 means AC 0 means DC, table ID 0-3
    // Number of codes for each length 16 bytes?

    // XX first nibble is number of zeros and second nibble is length of coeff
    // If more than 15 zeros then use code F0
    // Use 0 0 for end of block

    // Start of Scan
    write_start_of_scan(file);


    // Huffman data
    // fwrite(huffman_data, sizeof(huffman_data) * sizeof(unsigned char), sizeof(huffman_data), file);
    unsigned char *it;
    int i = 0;
    for (it = cvector_begin(huffman_data_vec); it != cvector_end(huffman_data_vec); ++it) {
        // printf("v[%d] = %c\n", i, *it);
        fputc(*it, file);
        ++i;
    }
    // End of Image
    fputc(0xFF, file);
    fputc(EOI, file);

    fclose(file);
    return 0;
}
