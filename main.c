#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Criar um array - Compartilhado --------
// Gerar números (1) em cada elemento - Paralelo --<
// Número do meio tem que ser o tamanho do array - Paralelo ====
// Barreira ===|===
// Verificar os máximos locais - Paralelo >----
// Comparar os máximos locais e obter o máximo de verdade - Compartilhado ----------

#define NUM_THREADS 4
#define max(a, b) (a > b ? a : b)

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("Wrong arguments. Please enter <amount_of_elements>\n");
        exit(0);
    }
    int threads;
    int size = atoi(argv[1]);
    int max_parcial[NUM_THREADS];

    printf("Vector size = %d\n", size);
    fflush(0);

    int *num = (int *) malloc(size * sizeof(int));

    omp_set_num_threads(NUM_THREADS);
    
    double wtime = omp_get_wtime();
    #pragma omp parallel
    {
        int nThreads = omp_get_num_threads();
        int ID = omp_get_thread_num();
        int partitionSize = (size / nThreads);
        
        int start = partitionSize * ID;
        int end = (ID != nThreads - 1 ? start + partitionSize : size);

        printf("%d: %d %d\n", ID, start, end);

        for (int i = start; i < end; ++i)
            num[i] = 1 + (i == size / 2) * (size - 1);

        #pragma omp barrier

        if (ID == 0)
            threads = nThreads;

        max_parcial[ID] = 0;
        for (int i = start; i < end; ++i)
            if (num[i] > max_parcial[ID])
                max_parcial[ID] = num[i];
    }

    int ans = 0;
    for (int i = 0; i < threads; i++)
        ans = max(ans, max_parcial[i]);

    wtime = omp_get_wtime() - wtime;
    
    printf("OMP array max local: Size = %d, Num_Threads = %d, max = %d, Elapsed wall clock time = %f\n", size, threads, ans, wtime);
    free(num);

    return 0;
}