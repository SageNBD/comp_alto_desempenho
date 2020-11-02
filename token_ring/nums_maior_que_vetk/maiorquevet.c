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

int  main(int argc, char *argv[])
{
	int npes, myrank, msgtag, src, dest, ret;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	msgtag = 1;

	if (myrank == 0)
	{	
		// O Processo 0 é encarregada de ler o stdin e enviar para cada uma dos outros processos as partições que serão encarregadas
		// de comparar com o limiar e mandar de volta para o processo 0 os índices dos valores que ultrapassaram esse limiar

		int n, k;
		scanf("%d", &n);
		scanf("%d", &k);

		int vet[n];
		for(int i = 0; i < n; i++)
        	scanf("%d", &vet[i]);
		
		int results[n];
		int count = 0;

		int start, end;
		int	partitionSize = (n/npes);
		int sendPartitionSize;
		for(int i = 1; i < npes; i++)
		{
			// Define os tamanhos e intervalos das partições
			start = partitionSize * i;
			end = i != (npes - 1) ? start + partitionSize : n;

			sendPartitionSize = end - start;
			int partition[sendPartitionSize];

			memcpy(partition, vet + start, sizeof(int) * sendPartitionSize);

			// Envia os dados
			MPI_Send((void*)&sendPartitionSize, 1, MPI_INT, i, msgtag, MPI_COMM_WORLD);
			MPI_Send((void*)&start, 1, MPI_INT, i, msgtag, MPI_COMM_WORLD);
			MPI_Send((void*)&(vet[k]), 1, MPI_INT, i, msgtag, MPI_COMM_WORLD);
			MPI_Send((void*)partition, sendPartitionSize, MPI_INT, i, msgtag, MPI_COMM_WORLD);
		}

		start = 0;

		// Calcula para a partição da própria thread 0
		for(int i = start; i < partitionSize; i++)
			if(vet[i] > vet[k])
				results[count++] = i;		

		int nElements;
		int* rcvBuffer = (int*) malloc(sizeof(int) * sendPartitionSize);
		for(int i = 1; i < npes; i++)
		{
			// Recebe primeiro quantos elementos foram encontrados, depois obtem os próprios elementos

			MPI_Recv((void*)&nElements, 1, MPI_INT, i, msgtag, MPI_COMM_WORLD, &status);
			if(nElements > 0)
			{
				MPI_Recv((void*)rcvBuffer, nElements, MPI_INT, i, msgtag, MPI_COMM_WORLD, &status);
				memcpy(results + count, rcvBuffer, sizeof(int) * nElements);
				count += nElements;
			}
		}

		printf("%d\n", count);
		for(int i = 0; i < count; i++)
			printf("%d ", results[i]);
		printf("\n");

		free(rcvBuffer);
	}
	else
	{
		int partitionSize, start, threshold;

		// Cada processo obtém os dados da sua partição e o limiar a se comparar com

		MPI_Recv((void*)&partitionSize, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);
		MPI_Recv((void*)&start, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);
		MPI_Recv((void*)&threshold, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);

		int partition[partitionSize];
		MPI_Recv((void*)partition, partitionSize, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);

		int sendBuffer[partitionSize];
		int nElements = 0;

		for(int i = 0; i < partitionSize; i++)
			if(partition[i] > threshold)
				sendBuffer[nElements++] = i + start;		

		// Manda quantos valores foram encontrados, e os valores em si
		MPI_Send((void*)&nElements, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
		if(nElements > 0)
			MPI_Send((void*)sendBuffer, nElements, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
	}
	
	fflush(0);
	MPI_Finalize();
	return 0;
}