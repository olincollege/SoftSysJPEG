#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>


#define MAT_SIZE 8

int sample_slice[64] = {154, 123, 123, 123, 123, 123, 123, 136,
                            192, 180, 136, 154, 154, 154, 136, 110,
                            254, 198, 154, 154, 180, 154, 123, 123,
                            239, 180, 136, 180, 180, 166, 123, 123,
                            180, 154, 136, 167, 166, 149, 136, 136,
                            128, 136, 123, 136, 154, 180, 198, 145,
                            123, 105, 110, 149, 136, 136, 180, 166,
                            110, 136, 123, 123, 123, 136, 154, 136};

void slice_to_mat(int* arr, gsl_matrix *mat){
    for (int i = 0; i < 64; i++){
        gsl_matrix_set(mat, i%8, i/8, arr[i]-128);
    }
}

void mat_to_slice(int* arr, gsl_matrix *mat){
    for (int i = 0; i < 64; i++){
        arr[i] = round(gsl_matrix_get(mat, i%8, i/8));
    }
}

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

