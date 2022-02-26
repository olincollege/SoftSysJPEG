#include <gsl/gsl_matrix.h>
int main(int argc, char const* argv[]) {
    // Make 8x8 test matrix
    // Make 8x8 quantization table
    // Elementwise divide
    // Elementwise round down
    // Print
    gsl_matrix* myMatr = gsl_matrix_alloc(10, 20);

    // matrix of 8s
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 20; j++) gsl_matrix_set(myMatr, i, j, 8);

    int elem = gsl_matrix_get(myMatr, 3, 4);

    gsl_matrix_free(myMatr);
    return 0;
}
