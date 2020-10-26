/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <omp.h>
#include <limits.h>

#define NUM_THREADS 8

int main(int argc, char *argv[])
{
    int n, k;
    scanf("%d %d", &n, &k);

    // reads all numbers
    int mat[n][n];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &mat[i][j]);

    int threads, min[n], min_parcial[n];
    for (int i = 0; i < n; i++)
    {
        min[i] = INT_MAX;
        min_parcial[i] = INT_MAX;
    }

    //sets threads number
    omp_set_num_threads(NUM_THREADS);

    //Initialize lock
    omp_lock_t mLock[n];
    
    for (int i = 0; i < n; i++)
        omp_init_lock(&mLock[i]);

    //Start of parallel region, set shared and private variables
    #pragma omp parallel shared(mat, threads, n, k, min) firstprivate (min_parcial)
    {
        
        //get current thread
        int ID = omp_get_thread_num();
        //get the actual number of threads
        int nThreads = omp_get_num_threads();

        if(ID == 0)
            threads = nThreads;

        for(int i = 0; i < n; i++)
        {
            //get current thread
            int ID = omp_get_thread_num();

            //calculates the region of the array that each thread will work 
            int partitionSize = (n/nThreads);
            partitionSize = partitionSize == 0 ? 1 : partitionSize;
            int start = partitionSize * ID;
            int end = (ID != nThreads - 1 ? start + partitionSize : n);

            //each thread search for its min value
            for(int j = start; j < end; j++)
                if(mat[i][j] > mat[i][k] && mat[i][j] < min_parcial[i])
                    min_parcial[i] = mat[i][j];

            //sets lock, defining the critical region, so we can update the min of the array
            omp_set_lock(&mLock[i]);
                if(min_parcial[i] < min[i])
                    min[i] = min_parcial[i];
            omp_unset_lock(&mLock[i]);
        }

    }

    for (int i = 0; i < n; i++)
    {
        //destroy lock
        omp_destroy_lock(&mLock[i]);

        if (min[i] == INT_MAX)
            min[i] = -1;

        printf("%d ", min[i]);
    }

}