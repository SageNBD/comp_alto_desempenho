/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

/* 
 * int **alloc_matrix(int r, int c)
 * 
 * Allocates and fills matrix. Here we swap rows and columns so that it's easier for the calculations, 
 * since they will be per column instead of per row.
 * 
 * @param   int     r   number of rows
 * @param   int     c   number of columns
 * 
 * @return: int**   matrix:   dynamically allocated matrix
 */      
double **alloc_matrix(int r, int c)
{
    double **mat = (double **) malloc(sizeof(double *) * c);

    for (int i = 0; i < c; ++i) 
        mat[i] = (double *) malloc(sizeof(double) * r);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            scanf("%lf", &mat[j][i]); // swaps rows and columns
        }
    }

    return mat;
}

/*
 * int **copy_matrix(int **mat, int r, int c)
 * 
 * Creates a copy of a matrix, so that we don't change the original one.
 * 
 * @param   int**   mat     original matrix to be copied
 * @param   int     r       number of rows   
 * @param   int     c       number of columns   
 * 
 * @return  int**   new_mat    copy of dynamically allocated matrix
 */
double **copy_matrix(double **mat, int r, int c)
{
    double **new_mat = (double **) malloc(sizeof(double *) * c);

    for (int i = 0; i < c; ++i) 
    {
        new_mat[i] = (double *) malloc(sizeof(double) * r);
        memcpy(new_mat[i], mat[i], sizeof(double) * r);
    }

    return new_mat;
}

/*
 * void destroy_matrix(int r, void **mat)
 * 
 * Frees dynamically allocateed matrix
 * 
 * @param   int     r       number of elements of each array
 * @param   void**  mat     dynamically allocated matrix
 * 
 * @return  NONE
 */
void destroy_matrix(int r, void **mat)
{
    for (int i = 0; i < r; ++i) free(mat[i]);
    free(mat);
}