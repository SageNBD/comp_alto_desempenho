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

#define NUM_THREADS 8

int main(int argc, char *argv[])
{
    int len;
    scanf("%d", &len);
    fgetc(stdin);

    char buffer[len];

    fgets(buffer, len + 1, stdin);
    buffer[len] = '\0';

    char *delim = {" "};
    char *token = strtok(buffer, delim);
    int tam_palavras[len];
    int num_palavras = 0;

    // For each word in the sentence, we save it's size
    while (token != NULL)
    {
        tam_palavras[num_palavras++] = strlen(token);
        token = strtok(NULL, delim);
    }

    int threads, max = 0, max_parcial = 0;

    //sets threads number
    omp_set_num_threads(NUM_THREADS);

    //Initialize lock
    omp_lock_t mLock;
    omp_init_lock(&mLock);

    //Start of parallel region, set shared and private variables
    #pragma omp parallel shared(tam_palavras, threads, num_palavras, max) private (max_parcial)
    {
        //get the actual number of threads
        int nThreads = omp_get_num_threads();
        //get current thread
        int ID = omp_get_thread_num();

        //calculates the region of the array that each thread will work 
        int partitionSize = (num_palavras/nThreads);
        int start = partitionSize * ID;
        int end = (ID != nThreads - 1 ? start + partitionSize : num_palavras);

        if(ID == 0)
            threads = nThreads;

        //each thread search for its max value
        for(int i = start; i < end; i++)
            if(tam_palavras[i] > max_parcial)
                max_parcial = tam_palavras[i];

        //sets lock, defining the critical region, so we can update the max of the array
        omp_set_lock(&mLock);
            if(max_parcial > max)
                max = max_parcial;
        omp_unset_lock(&mLock);
    }

    //destroy lock
    omp_destroy_lock(&mLock);

    printf("%d", max);
}