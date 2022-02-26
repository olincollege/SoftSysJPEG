#include <gsl/gsl_matrix.h>
#include <math.h>
#define rows 8
#define cols 8
int main(int argc, char const* argv[]) {
    // Make 8x8 test matrix
    // Make 8x8 quantization table
    // Elementwise divide
    // Elementwise round down
    // Print

    const double data[rows*cols] = { 
        313, 56, -27, 18, 78, -60, 27, -27,
        -38, -27, 13, 44, 32, -1, -24, -10,
        -20, -17, 10, 33, 21, -6, -16, -9,
        -10, -8, 9, 17, 9, -10, -13, 1,
        -6, 1, 6, 4, -3, -7, -5, 5,
        2, 3, 0, -3, -7, -4, 0, 3,
        4, 4, -1, -2, -9, 0, 2, 4,
        3, 1, 0, -4, -2, -1, 3, 1 
    };  
    const double quantization_data[rows*cols] = { 
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 11, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
    };  
    gsl_matrix_const_view mat = gsl_matrix_const_view_array( data, rows, cols );
    gsl_matrix_const_view quantization_table = gsl_matrix_const_view_array( quantization_data, rows, cols );
    gsl_matrix_div_elements(&mat.matrix, &quantization_table.matrix);
    for ( size_t row = 0; row < rows; ++row ) { 
        for ( size_t col = 0; col < cols; ++col ) { 
            printf( " %i", (int)round(gsl_matrix_get( &mat.matrix, row, col ) ));
        }
        printf( "\n" );
    }
    return 0;
}
