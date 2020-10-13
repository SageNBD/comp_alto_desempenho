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
#include <omp.h>

int **alloc_matrix(int r, int c) {
    int **mat = (int **) malloc(sizeof(int *) * c);

    for (int i = 0; i < c; ++i) 
        mat[i] = (int *) malloc(sizeof(int) * r);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            scanf("%d", &mat[j][i]);
        }
    }

    return mat;
}

int **copy_matrix(int **mat, int r, int c) {
    int **new_mat = (int **) malloc(sizeof(int *) * c);

    for (int i = 0; i < c; ++i) 
    {
        new_mat[i] = (int *) malloc(sizeof(int) * r);
        memcpy(new_mat[i], mat[i], sizeof(int) * r);
    }

    return new_mat;
}

void print_matrix(int **mat, int r, int c) {
    for (int i = 0; i < r; ++i) 
    {
        for (int j = 0; j < c; ++j) 
        {
            printf("%d ", mat[j][i]);
        }
        printf("\n");
    }
}

void destroy_matrix(int r, int **mat) {
    for (int i = 0; i < r; ++i) free(mat[i]);
    free(mat);
}

double average(int *vec, int c) 
{
    double sum = 0;
    for (int i = 0; i < c; ++i)
        sum += vec[i];
    return sum / (double) c;
}

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

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b );
}

double median(int *vec, int size) 
{
    qsort(vec, size, sizeof(int), compare);
    return vec[size / 2];
}

int mode(int *vec, int size) 
{
    int num_elements = 0;
    for (int i = 0; i < size; ++i) 
        if (vec[i] > num_elements)
            num_elements = vec[i];

    int *freq = (int *) calloc(num_elements, sizeof(int));
    int *pos = (int *) calloc(num_elements, sizeof(int));

    int max = 0, current_mode;
    for (int i = 0; i < size; ++i) 
    {
        if (pos[vec[i] - 1] == 0)
            pos[vec[i] - 1] = i + 1;

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

    return (max != 1 ? current_mode : -1);
}

void create_metrics(int **mat, int c, int nrows, int ncols) 
{
    #pragma omp task
    {
        printf("thread: %d ", omp_get_thread_num());
        double avg = average(mat[c], nrows);
        printf("aritmética da coluna %d: %lf\n", c, avg);
    }
    #pragma omp task
    {
        printf("thread: %d ", omp_get_thread_num());
        double harm = harmonic(mat[c], nrows);
        printf("harmônica da coluna %d: %lf\n", c, harm);
    }
    #pragma omp task
    {
        int **new_mat = copy_matrix(mat, nrows, ncols);
        int md = median(new_mat[c], nrows);
        printf("mediana da coluna %d: %d\n", c, md);
    }
    #pragma omp task
    {
        int moda = mode(mat[c], nrows);
        printf("moda da coluna %d: %d\n", c, moda);
    }
}

int main(int argc, char *argv[]) 
{
    int nrows, ncols;

    scanf("%d %d", &nrows, &ncols);

    int **mat = alloc_matrix(nrows, ncols);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        #pragma omp for
            for (int i = 0; i < ncols; i++)
                #pragma omp task
                    create_metrics(mat, i, nrows, ncols);
    }

    print_matrix(mat, nrows, ncols);

    return 0;
}