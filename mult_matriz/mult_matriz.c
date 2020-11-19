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

#define N 32
#define NUMTHREADS 4

int  main(int argc, char *argv[])
{
	int npes, myrank, msgtag, src, dest, ret;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	msgtag = 1;
	int token;

	if (myrank == 0)
	{
		int A[N][N], BTrans[N][N], C[N][N];
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				A[i][j] = rand() % 2;
				BTrans[j][i] = rand() % 2;
			}
		}

		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				if(i == 0 && j == 0)
					continue;

				dest = N*i + j;
				MPI_Send((void*)A[i], N, MPI_INT, dest, msgtag, MPI_COMM_WORLD);
				MPI_Send((void*)BTrans[j], N, MPI_INT, dest, msgtag, MPI_COMM_WORLD);
			}
		}

		#pragma omp parallel
		{
			//get the actual number of threads
			int nThreads = omp_get_num_threads();
			//get current thread
			int ID = omp_get_thread_num();

			//calculates the region of the array that each thread will work 
			int partitionSize = (N/nThreads);
			int start = partitionSize * ID;
			int end = (ID != nThreads - 1 ? start + partitionSize : N);

			C[0][0] = 0;
			for(int i = start; i < end; i++)
				#pragma omp atomic
					C[0][0] += A[0][i] + B[0][i];
		}

		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				if(i == 0 && j == 0)
				{
					printf("%d ", C[0][0]);
					continue;
				}

				src = N*i + j;
				int parc_result;
				MPI_Recv((void*)&parc_result, 1, MPI_INT, src, msgtag, MPI_COMM_WORLD, &status);
				C[i][j] = parc_result;
				printf("%d ", C[i][j]);
			}
			printf("\n");
		}
	}
	else
	{
		src = myrank;
		int result = 0;
		int linha[N], coluna[N];
		
		MPI_Recv((void*)linha, N, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);
		MPI_Recv((void*)coluna, N, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);

		//Start of parallel region, set shared and private variables
		#pragma omp parallel
		{
			//get the actual number of threads
			int nThreads = omp_get_num_threads();
			//get current thread
			int ID = omp_get_thread_num();

			//calculates the region of the array that each thread will work 
			int partitionSize = (N/nThreads);
			int start = partitionSize * ID;
			int end = (ID != nThreads - 1 ? start + partitionSize : N);

			for(int i = start; i < end; i++)
				#pragma omp atomic
					result += linha[i] + coluna[i];
		}

		MPI_Send((void*)&result, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}