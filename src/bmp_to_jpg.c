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
    uint16_t code = 0;
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
    }
    (*data)[cvector_size((*data))-1] |= (bit & 1) << (7 - nextBit);
    nextBit = (nextBit + 1) % 8;
    if (nextBit == 0 && (*data)[cvector_size((*data))-1] == 0xFF) {
        cvector_push_back((*data), 0);
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
            return;
        }

        while (numZeroes >= 16) {
            getCode(acTable, 0xF0, &code, &codeLength);
        // printf("\n\n%i %i 0chunk\n", code, coeff);
        // printf("%i %i\n", codeLength, coeffLength);
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
        byte symbol = (numZeroes << 4) | coeffLength;
        getCode(acTable, symbol, &code, &codeLength);
        // printf("\n\n%i %i\n", code, coeff);
        // printf("%i %i\n", codeLength, coeffLength);
        writeBits(code, codeLength, data);
        writeBits(coeff, coeffLength, data);
    }
}

void encode_huffman(Image * image, unsigned char ** huffmanData) {

    int previousDCs[3] = { 0 };

    for (uint i = 0; i < 3; ++i) {
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
                        &(previousDCs[0]),
                        dcTables[0],
                        acTables[0]);
            encodeBlockComponent(
                        huffmanData,
                        image->blocks[y * image->blockWidth + x].cb,
                        &(previousDCs[1]),
                        dcTables[1],
                        acTables[1]);
            encodeBlockComponent(
                        huffmanData,
                        image->blocks[y * image->blockWidth + x].cr,
                        &(previousDCs[2]),
                        dcTables[2],
                        acTables[2]);
            
        }
    }

}

void print_blocks(Image * image) {
    for (size_t i = 0; i < image->blockHeight*image->blockWidth; i++)
    {
        for (size_t j = 0; j < 64; j++)
        {
            printf("%i,", image->blocks[i].cb[j]);
        }      
        printf("\n");  
    }
}

int main(int argc, char const *argv[])
{
    Image * image = malloc(sizeof(Image));
    BMP* bmp = bopen("include/data/beeeeeeg.bmp");
    // read in the bmp TODO figure out why only 24 bit images work
    if (read_bmp(bmp, image)) {
        printf("Error reading BMP\n");
        return EXIT_FAILURE;
    }
    bclose(bmp);
    // Process image data to jpeg

    rgb_to_ycrcb(image);

    dct(image);
    quantize(image, yQuantTable, CrCbQuantTable);
    print_blocks(image);
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
