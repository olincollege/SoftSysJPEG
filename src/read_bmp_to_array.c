#include <stdio.h>
#include <stdlib.h>

unsigned char *read_bmp_to_array(char *file_location, int *width, int *height,
                        int *padding, int *bitcount, int *size) {
    unsigned char *data = 0;
    unsigned char info[54] = {0};
    FILE *file = fopen(file_location, "rb");
    if (!file) {
        return 0;
    }
    fread(info, 1, 54, file);
    (*width) = *(int *)(info + 18);
    *height = *(int *)(info + 22);
    (*bitcount) = *(int *)(info + 28);
    (*size) = (((*width) * (*bitcount) + 31) / 32) * 4 * (*height);
    (*padding) = (*width) % 4;
    if ((*bitcount) != 24) {  // this code works for 24-bit bitmap only
        if (file) fclose(file);
        if (data) free(data);
        return 0;
    }

    data = malloc((*size));
    fread(data, 1, (*size), file);
    return data;
}

void read_channels (unsigned char * data, int height, int width, int padding, int*red, int*green, int* blue) {
    for (int row = height - 1; row >= 0; row--) {
        for (int col = 0; col < width; col++) {
            int p = (row * width + col) * 3 + row * padding;
            *((red+row*width) + col) = data[p + 0];
            *((green+row*width) + col) = data[p + 1];
            *((blue+row*width) + col) = data[p + 2];
            // red[row][col] = data[p + 0];
            // green[row][col] = data[p + 1];
            // blue[row][col] = data[p + 2];
        }
    }
}

void rgb_to_ycrcb(int height, int width, int* red, int* green, int* blue, int* y, int* cb, int* cr){
    int val;
    int r,g,b;
    // r -> y
    // g -> cb
    // b -> cr
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            r = *((red+i*j)+j);
            g = *((green+i*j)+j);
            b = *((blue+i*j)+j);
            *((y+i*j)+j) = 16+ (((r<<6)+(r<<1)+(g<<7)+g+(b<<4)+(b<<3)+b)>>8);
            *((cb+i*j)+j) = 128 + ((-((r<<5)+(r<<2)+(r<<1))-((g<<6)+(g<<3)+(g<<1))+(b<<7)-(b<<1))>>8);
            *((cr+i*j)+j) = 128 + (((r<<7)-(r<<4)-((g<<6)+(g<<5)-(g<<1))-((b<<4)+(b<<1)))>>8); 
        }
    }
}

void chroma_downsample(int* cb, int* cr, int height, int width){
    int cb_avg;
    int cr_avg;
    // 4:2:2 downsampling
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width-1; col+2) {
            //average for each 2x1 sample block
            cb_avg = (*((cb+row*width)+col)+ *((cb+row+1*width)+col))/2;
            *((cb+row*width)+col) = cb_avg;
            *((cb+row+1*width)+col) = cb_avg;
            cr_avg = (*((cr+row*width)+col)+ *((cr+row+1*width)+col))/2;
            *((cr+row*width)+col) = cr_avg;
            *((cr+row+1*width)+col) = cr_avg;
        }
    }
}

int main(int argc, char const *argv[]) {
    int width, height, padding, bitcount, size;
    unsigned char  *data = read_bmp_to_array("include/data/test.bmp", &width, &height,
                                   &padding, &bitcount, &size);
    int red[height][width];
    int green[height][width];
    int blue[height][width];
    read_channels(data, height, width, padding, (int*)red, (int*)green, (int*)blue);
    int y[height][width];
    int cb[height][width];
    int cr[height][width];
    rgb_to_ycrcb(height, width, (int*)red, (int*)green, (int*)blue, (int*)y, (int*)cb, (int*)cr);
    int i, j;
    // Print out array for testing
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            printf("%i ", red[i][j]);
        }
        printf("\n");
    }
}
