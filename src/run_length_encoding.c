#include <stdlib.h>
#include <stdio.h>
#define rows 8
#define cols 8

unsigned int countBits(unsigned int n)
{
   unsigned int count = 0;
   while (n)
   {
        count++;
        n >>= 1;
    }
    return count;
}

void run_line_encode(int* data, int _rows, int _cols) {
    u_int8_t up = 1;
    int row = 0;
    int col = 0;
    int zero_count = 0;
    for (size_t i = 0; i < _rows*_cols; i++)
    {
        int coeff = data[row*_rows + col];
        if (coeff != 0) {
            printf("[(%i %i) %i]\n", zero_count, countBits(coeff>0?coeff:-coeff), coeff);
            zero_count = 0;
        } else {
            zero_count++;
        }
        if (up) {
            row--;
            col++;
            if (col >=_cols) {
                up = 0;
                col--;
                row +=2;
            } else if (row <0) {
                up = 0;
                row++;
            }
        } else {
            row++;
            col--;
            if (row >=_rows) {
                up = 1;
                row--;
                col +=2;
            } else if (col <0) {
                up = 1;
                col++;
            }
        }
    }
    printf("[(0 0)]\n");

}


int main(int argc, char const *argv[])
{
    // int data[rows * cols] = { 20,5,-3,1,3,-2,1,0,-3,-2,1,2,1,0,0,0,-1,-1,1,1,1,0,0,0,-1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int data[rows * cols] = {90,-40,0,4,0,0,0,0,
                             0, 0, 0,0,0,0,0,0,
                             5, 0, 0,0,0,0,0,0,
                             0, 0, 0,0,0,0,0,0,
                             11,-3,0,0,0,0,0,0,
                             0, 0, 0,0,0,0,0,0,
                             0, 0, 0,0,0,0,0,0,
                             0, 0, 0,0,0,0,0,0};
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            printf("%i\t", data[i*rows + j]);
        }
        printf("\n");
    }
    run_line_encode(data,rows, cols);
      
    return 0;
}
