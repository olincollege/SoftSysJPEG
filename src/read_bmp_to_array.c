#include <stdio.h>
#include <stdlib.h>
#include "JPEG.h"

int read_bmp(BMP * bmp, Image * image);
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