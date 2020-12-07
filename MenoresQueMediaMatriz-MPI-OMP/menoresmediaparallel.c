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
        
        //Aloca a matriz
        MAT = (int *)malloc(dim * dim * sizeof(int));

        //Aloca um vetor para armazenar a quantide de nrs menores que a media por coluna
        vet_menor = (int *)malloc(dim * sizeof(int));
        vet_menor_buffer = (int *)malloc(dim * sizeof(int));
        
        // incializa com zero as posicoes de vet_menor
        for (j = 0; j< dim; j++)
        {
            vet_menor_buffer[j] = 0;
        }

        media_MAT = 0;
        //Lê a matriz MAT
        for(i=0;i<dim;i++)
        {
            for(j=0;j<dim;j++)
            {
                // Transpomos a matriz na leitura para facilitar a divisão entre os processos
                fscanf(inputfile, "%d ", &(MAT[j*dim+i]));
                media_MAT += MAT[j*dim+i];
            }
        }

        media_MAT = media_MAT / (double)(dim*dim);

        // fecha o arquivo de entrada
        fclose(inputfile);
	}

    MPI_Bcast(&dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&media_MAT, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int partitionSize = (dim/npes);
    int start = partitionSize * myrank * dim;
	int end = (myrank != npes - 1 ? start + partitionSize * dim : dim * dim);

    int sendcounts[npes], displacements[npes];
    for(int i = 0; i < (npes-1); i++)
    {
        sendcounts[i] = partitionSize * dim;
        displacements[i] = partitionSize * dim * i;
    }
    sendcounts[npes-1] = dim*dim - (partitionSize * dim * (npes-1));
    displacements[npes-1] = dim * partitionSize * (npes-1);

    int cols[end-start];

    MPI_Scatterv(MAT, sendcounts, displacements, MPI_INT, cols, end-start, MPI_INT, 0, MPI_COMM_WORLD);

    int nOfCols = (end - start) / dim;

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
                    vet_menor_buffer[myrank * partitionSize + i]++;
                }
            }
        }
    }

    int recvCounts[npes], displs[npes];
    for(int i = 0; i < npes - 1; i++)
    {
        recvCounts[i] = partitionSize;
        displs[i] = i * partitionSize;
    }
    recvCounts[npes-1] = dim - (npes-1) * partitionSize;
    displs[npes-1] = partitionSize * (npes-1);


    printf("aaaa");
    MPI_Gatherv(vet_menor, dim, MPI_INT, vet_menor_buffer, recvCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    printf("bbbb");

    if (myrank == 0)
    {
        for (int i = 0; i < dim; i++)
            printf("%d ", vet_menor[i]);
    }

	MPI_Finalize();
	return 0;
}