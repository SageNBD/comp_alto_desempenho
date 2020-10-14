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
#include "matrix.h"
#include "stats.h"

#define NUM_THREADS 8

// Creates and organizes the tasks for all the metrics
void generate_metrics(double **mat, int c, int nrows, int ncols, StatsMetrics metrics)
{
    #pragma omp task
        metrics.harms[c] = harmonic(mat[c], nrows);
    #pragma omp task
    {
        double **new_mat = copy_matrix(mat, nrows, ncols);
        metrics.medians[c] = median(new_mat[c], nrows);
    }
    #pragma omp task
        metrics.modes[c] = mode(mat[c], nrows);
    #pragma omp task
    {
        metrics.avgs[c] = average(mat[c], nrows);
        metrics.variances[c] = variance(mat[c], nrows, metrics.avgs[c]);
        metrics.std_deviations[c] = sqrt(metrics.variances[c]);
        metrics.var_coefs[c] = metrics.std_deviations[c] / metrics.avgs[c];
    }
}

int main(int argc, char *argv[]) 
{
    int nrows, ncols, threads;

    scanf("%d %d", &nrows, &ncols);

    double **mat = alloc_matrix(nrows, ncols);
    StatsMetrics metrics = create_stats_metrics(ncols);
    
    //sets threads number
    omp_set_num_threads(NUM_THREADS);

    //start counting time
    double wtime = omp_get_wtime();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        if(id == 0)
            threads = num_threads;

        // Omp for to make sure no more than 1 thread creates the tasks for a column
        #pragma omp for
            for (int i = 0; i < ncols; i++)
                #pragma omp task
                    generate_metrics(mat, i, nrows, ncols, metrics);
    }

    print_all_metrics(metrics);
     //stop counting time
    wtime = omp_get_wtime() - wtime;

    printf("\nElapsed wall clock time = %f\n", wtime);
    printf("\nNum_Threads = %d \n", threads);

    // Frees matrixes
     destroy_matrix(ncols, (void**)mat);
     deallocate_StatsMetrics(metrics);

    return 0;
}