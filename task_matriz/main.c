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
#include <math.h>
#include <omp.h>

// Allocates and fills matrix. Here we swap rows and columns so that it's easier for the calculations, 
// since they will be per column instead of per row.
int **alloc_matrix(int r, int c)
{
    int **mat = (int **) malloc(sizeof(int *) * c);

    for (int i = 0; i < c; ++i) 
        mat[i] = (int *) malloc(sizeof(int) * r);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            scanf("%d", &mat[j][i]); //swaps rows and columns
        }
    }

    return mat;
}

// Creates a copy of a matrix, so that we don't change the original one.
int **copy_matrix(int **mat, int r, int c)
{
    int **new_mat = (int **) malloc(sizeof(int *) * c);

    for (int i = 0; i < c; ++i) 
    {
        new_mat[i] = (int *) malloc(sizeof(int) * r);
        memcpy(new_mat[i], mat[i], sizeof(int) * r);
    }

    return new_mat;
}

// Frees matrix
void destroy_matrix(int r, void **mat)
{
    for (int i = 0; i < r; ++i) free(mat[i]);
    free(mat);
}

// Gets arithmetic mean
double average(int *vec, int c) 
{
    double sum = 0;
    for (int i = 0; i < c; ++i)
        sum += vec[i];
    return sum / (double) c;
}

// Gets harmonic mean
double harmonic(int *vec, int size) 
{
    double sum = 0;
    for (int i = 0; i < size; ++i)
    {
        if (vec[i] == 0) return 0;
        sum += 1 / (double) vec[i];
    }
    return (double) size / sum;
}

// Utility method for qsort
int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b );
}

// Sorts the array, gets center element to get the median
double median(int *vec, int size) 
{
    qsort(vec, size, sizeof(int), compare);
    return size % 2 != 0 ? vec[size / 2] : ((double)vec[size / 2] + vec[(size/2)-1])/2; // Handles even arrays
}

// Gets the variance, given the pre-calculated average
double variance(int *vec, int size, double avg)
{
    double sum = 0;
    for(int i = 0; i < size; i++)
        sum += pow(vec[i] - avg, 2);

    return sum / (size - 1);
}

// Gets mode, treats input as integers
int mode(int *vec, int size) 
{
    int num_elements = 0;
    for (int i = 0; i < size; ++i) 
        if (vec[i] > num_elements)
            num_elements = vec[i];

    int *freq = (int *) calloc(num_elements, sizeof(int)); // Array to indicate the frequency of a value
    int *pos = (int *) calloc(num_elements, sizeof(int)); // Array to indicate the very first time a value was found

    int max = 0, current_mode;
    for (int i = 0; i < size; ++i) 
    {
        if (pos[vec[i] - 1] == 0)
            pos[vec[i] - 1] = i + 1;

        // If two values are competing for being the mode, get the one that appeared first in the array
        if (++freq[vec[i] - 1] == max && pos[vec[i] - 1] < pos[current_mode])
        {
            max = freq[vec[i] - 1];
            current_mode = vec[i];
        }
        else if (freq[vec[i] - 1] > max) 
        {                
            max = freq[vec[i] - 1];
            current_mode = vec[i];
        }
    }
    
    free(freq);
    free(pos);

    return (max != 1 ? current_mode : -1); // If no element is ever repeated, return -1
}

// Creates and organizes the tasks for all the metrics
void create_metrics(int **mat, int c, int nrows, int ncols,
 double *avgs, double *harms, double *medians, double *modes, double *variances, double *std_deviations, double *var_coefs) 
{
    #pragma omp task
        harms[c] = harmonic(mat[c], nrows);
    #pragma omp task
    {
        int **new_mat = copy_matrix(mat, nrows, ncols);
        medians[c] = median(new_mat[c], nrows);
    }
    #pragma omp task
        modes[c] = mode(mat[c], nrows);
    #pragma omp task
    {
        avgs[c] = average(mat[c], nrows);
        variances[c] = variance(mat[c], nrows, avgs[c]);
        std_deviations[c] = sqrt(variances[c]);
        var_coefs[c] = variances[c] / avgs[c];
    }
}

int main(int argc, char *argv[]) 
{
    int nrows, ncols;

    scanf("%d %d", &nrows, &ncols);

    int **mat = alloc_matrix(nrows, ncols);

    // Allocates matrix for the 7 metrics.
    double **metrics = (double**)malloc(sizeof(double*) * 7);
    for(int i = 0; i < 7; i++)
        metrics[i] = (double*)malloc(sizeof(double) * ncols);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Omp for to make sure no more than 1 thread creates the tasks for a column
        #pragma omp for
            for (int i = 0; i < ncols; i++)
                #pragma omp task
                    create_metrics(mat, i, nrows, ncols,
                     metrics[0], metrics[1], metrics[2], metrics[3], metrics[4], metrics[5], metrics[6]);
    }

    // Prints the 7 metrics
    for(int i = 0; i < 7; i++)
    {
        for(int j = 0; j < ncols; j++)
            printf("%.1lf ", round(metrics[i][j]*10)/10);
        printf("\n");
    }

    // Frees matrixes
    destroy_matrix(ncols, (void**)mat);
    destroy_matrix(7, (void**)metrics);

    return 0;
}