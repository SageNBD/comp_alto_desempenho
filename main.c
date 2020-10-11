
/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Criar um array - Compartilhado --------
// Gerar números (1) em cada elemento - Paralelo --<
// Número do meio tem que ser o tamanho do array - Paralelo ====
// Barreira ===|===
// Verificar os máximos locais - Paralelo >----
// Comparar os máximos locais e obter o máximo de verdade - Compartilhado ----------

#define NUM_THREADS 8


int main(int argc, char const *argv[])
{
    //gets array size
    if(argc != 2)
    {
        printf("Wrong arguments. Please enter <amount_of_elements>\n");
        exit(0);
    }

    double wtime;
    int size = atoi(argv[1]);
    int threads, max = 0, max_parcial = 0;
    
    printf("Vector size = %d\n", size);
    fflush(0);

    //create array 
    int *num = (int*) malloc(size * sizeof(int));

    //sets threads number
    omp_set_num_threads(NUM_THREADS);

    //Initialize lock
    omp_lock_t mLock;
    omp_init_lock(&mLock);

    //start counting time
    wtime = omp_get_wtime();

    //Start of parallel region, set shared and private variables
    #pragma omp parallel shared(num, threads, size, max) private (max_parcial)
    {
        //get the actual number of threads
        int nThreads = omp_get_num_threads();
        //get current thread
        int ID = omp_get_thread_num();

        //calculates the region of the array that each thread will work 
        int partitionSize = (size/nThreads);
        int start = partitionSize * ID;
        int end = (ID != nThreads - 1 ? start + partitionSize : size);
        
        //each thread fill its part of the array. If i is in the middle of the array,
        //it becomes equal "size"
        for(int i = start; i < end; i++)
            num[i] = 1 + (i == size/2)*(size - 1);
        
        //syncronize threads before searching for the max local value 
        #pragma omp barrier

        if(ID == 0)
            threads = nThreads;

        //each thread search for its max value
        for(int i = start; i < end; i++)
            if(num[i] > max_parcial)
                max_parcial = num[i];

        //sets lock, defining the critical region, so we can update the max of the array
        omp_set_lock(&mLock);
            if(max_parcial > max)
                max = max_parcial;
        omp_unset_lock(&mLock);
    }

    //destroy lock
    omp_destroy_lock(&mLock);
    //stop counting time
    wtime = omp_get_wtime() - wtime;
    

    printf("OMP array max local: Size = %d, Num_Threads = %d, max = %d, Elapsed wall clock time = %f\n", size, threads, max, wtime);
    free(num);

    return 0;
}