/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "omp.h"

int  main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Nr errado de argumentos. Execute passando <arq_entrada> como argumento. \n");
        exit(-1);
    }

    //Declara a matriz MAT e o vetor vet_menor
    int *MAT, *vet_menor, *vet_menor_buffer;

    //Declara as variáveis de índice
    int i,j,dim;
	
    //Declara a media para MAT
    double media_MAT;

    FILE *inputfile;     // handler para o arquivo de entrada
    char *inputfilename;  // nome do arquivo de entrada

	int npes, myrank, msgtag, src, dest, ret;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	msgtag = 1;
	
	// // Calculates partitions for process
	// int partitionSize = (dim*dim/npes);
	// int start = partitionSize * myrank;
	// int end = (myrank != npes - 1 ? start + partitionSize : dim*dim);

	if (myrank == 0)
	{
        inputfilename = (char*) malloc (256*sizeof(char));
        strcpy(inputfilename,argv[1]);

        if ((inputfile=fopen(inputfilename,"r")) == 0)
        {
            printf("Mestre: Erro ao abrir arquivo de entrada %s. Saindo. \n", inputfilename);
            exit(-1);
        }
        
        fscanf(inputfile, "%d\n", &dim); //Lê a dimensão de MAT
    }

    //Aloca a matriz
    MAT = (int *)malloc(dim * dim * sizeof(int));
        
    if (myrank == 0)
	{
        //Aloca um vetor para armazenar a quantide de nrs menores que a media por coluna
        vet_menor = (int *)malloc(dim * sizeof(int));

        // incializa com zero as posicoes de vet_menor
        for (j = 0; j< dim; j++)
        {
            vet_menor[j] = 0;
        }

        //Lê a matriz MAT
        for(i=0;i<dim;i++)
        {
            for(j=0;j<dim;j++)
            {
                // Transpomos a matriz na leitura para facilitar a divisão entre os processos
                fscanf(inputfile, "%d ", &(MAT[j*dim+i]));
            }
        }

        // fecha o arquivo de entrada
        fclose(inputfile);
	}

    MPI_Bcast(&dim, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int partitionSize = (dim/npes);
    int start = partitionSize * myrank * dim;
	int end = (myrank != npes - 1 ? start + partitionSize * dim : dim * dim);

    int sendcounts[npes], displacements_scatter[npes];
    for(int i = 0; i < (npes-1); i++)
    {
        sendcounts[i] = partitionSize * dim;
        displacements_scatter[i] = partitionSize * dim * i;
    }
    sendcounts[npes-1] = dim*dim - (partitionSize * dim * (npes-1));
    displacements_scatter[npes-1] = dim * partitionSize * (npes-1);

    int cols[end-start];

    MPI_Scatterv(MAT, sendcounts, displacements_scatter, MPI_INT, cols, end-start, MPI_INT, 0, MPI_COMM_WORLD);

    int sum_local = 0;
    #pragma omp parallel
    {
        #pragma omp for
        for (int j = 0; j < end-start; j++)
        { 
            #pragma omp atomic
            sum_local += cols[j];
        }
    }

    int recvcounts[npes], displacements_gather[npes];
    int* sums;
    if(myrank == 0)
    {
        sums = (int *)malloc(npes * sizeof(int));

        for(int i = 0; i < npes; i++)
        {
            recvcounts[i] = 1;
            displacements_gather[i] = i;
        }
    }

    // Faz o Gather.
    MPI_Gatherv(&sum_local, 1, MPI_INT, sums, recvcounts, displacements_gather, MPI_INT, 0, MPI_COMM_WORLD);

    media_MAT = 0;
    if(myrank == 0)
    {
        for(int i = 0; i < npes; i++)
            media_MAT += sums[i];

        media_MAT = media_MAT / (double)(dim*dim);
    }


    MPI_Bcast(&media_MAT, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int i = 0; i < (npes-1); i++)
    {
        sendcounts[i] = partitionSize * dim;
        displacements_scatter[i] = partitionSize * dim * i;
    }
    sendcounts[npes-1] = dim*dim - (partitionSize * dim * (npes-1));
    displacements_scatter[npes-1] = dim * partitionSize * (npes-1);

    MPI_Scatterv(MAT, sendcounts, displacements_scatter, MPI_INT, cols, end-start, MPI_INT, 0, MPI_COMM_WORLD);

    int nOfCols = (end - start) / dim;

    vet_menor_buffer = (int *)malloc(nOfCols * sizeof(int));

    // incializa com zero as posicoes de vet_menor
    for (j = 0; j< nOfCols; j++)
    {
        vet_menor_buffer[j] = 0;
    }

    for (int i = 0; i < nOfCols; i++)   
    {     
        #pragma omp parallel
        {
            #pragma omp for
            for (int j = 0; j < dim; j++)
            {
                if (cols[i*dim+j] < media_MAT)
                {
                    #pragma omp atomic
                    vet_menor_buffer[i]++;
                }
            }
        }
    }

    if(myrank == 0)
    {
        for(int i = 0; i < npes - 1; i++)
        {
            recvcounts[i] = partitionSize;
            displacements_gather[i] = i * partitionSize;
        }
        recvcounts[npes-1] = dim - ((npes-1) * partitionSize);
        displacements_gather[npes-1] = partitionSize * (npes-1);
    }

    MPI_Gatherv(vet_menor_buffer, nOfCols, MPI_INT, vet_menor, recvcounts, displacements_gather, MPI_INT, 0, MPI_COMM_WORLD);

    if (myrank == 0)
    {
        for (int i = 0; i < dim; i++)
            printf("%d ", vet_menor[i]);
        printf("\n");
    }

	MPI_Finalize();
	return 0;
}