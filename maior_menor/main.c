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
    int vet[n];
    for (int i = 0; i < n; i++)
        scanf("%d", &vet[i]);

    int threads, min = INT_MAX, min_parcial = INT_MAX;

    //sets threads number
    omp_set_num_threads(NUM_THREADS);

    //Initialize lock
    omp_lock_t mLock;
    omp_init_lock(&mLock);

    //Start of parallel region, set shared and private variables
    #pragma omp parallel shared(vet, threads, n, k, min) private (min_parcial)
    {
        // set min as maximum value, so that it will be replaced with any number in the first iteration
        min_parcial = INT_MAX;

        //get the actual number of threads
        int nThreads = omp_get_num_threads();
        //get current thread
        int ID = omp_get_thread_num();

        //calculates the region of the array that each thread will work 
        int partitionSize = (n/nThreads);
        int start = partitionSize * ID;
        int end = (ID != nThreads - 1 ? start + partitionSize : n);

        if(ID == 0)
            threads = nThreads;

        //each thread search for its min value
        for(int i = start; i < end; i++)
            if(vet[i] > vet[k] && vet[i] < min_parcial)
                min_parcial = vet[i];

        //sets lock, defining the critical region, so we can update the min of the array
        omp_set_lock(&mLock);
            if(min_parcial < min)
                min = min_parcial;
        omp_unset_lock(&mLock);
    }

    //destroy lock
    omp_destroy_lock(&mLock);

    if (min == INT_MAX)
        min = -1;

    printf("%d", min);
}