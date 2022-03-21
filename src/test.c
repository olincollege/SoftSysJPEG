#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

int main(int argc, char** argv)
{

    // if (argc != 3)
    // {
    //     fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
    //     exit(1);
    // }

    // Read image into BMP struct
    BMP* bmp = bopen(argv[1]);

    unsigned int i, j, width, height;
    unsigned char r, g, b;

    // Gets image width in pixels
    width = get_width(bmp);

    // Gets image height in pixels
    height = get_height(bmp);

    for (i = height-1; height>i ; i--)
    {
        printf("\n");
        for (j = 0; j < width; j++)
        {
            // Gets pixel rgb values at point (x, y)
            get_pixel_rgb(bmp, j, i, &r, &g, &b);
            printf("%i ", (int)r);
            // Sets pixel rgb values at point (x, y)
            // set_pixel_rgb(bmp, x, y, 255 - r, 255 - g, 255 - b);
        }
    }

    // Write bmp contents to file
    // bwrite(bmp, argv[2]);

    // Free memory
    bclose(bmp);

    return 0;
}