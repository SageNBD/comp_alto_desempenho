/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

/*
* 				  - PCAM - 
* |=========== Particionamento ===========|
*	Seguindo um particionamento por dados e lógica de multiplicação de matrizes que multiplica as linhas de A pelas colunas de B, as n linhas da matriz A vão operar cada uma por n colunas da matriz B, para obter os n x n = n2 resultados que compõem a matriz C. Vamos chamar essas tarefas de tarefas de particionamento.
*	Cada uma dessas n2tarefas de particionamento vão ser ainda divididas cada uma em n multiplicações (elemento da linha de A por elemento da coluna de B) que serão somadas para obter cada resultado, totalizando em n3tarefas. Vamos chamar essas n tarefas adicionais para cada uma das tarefas de particionamento de tarefas de multiplicação.
*	Vamos ter ainda uma tarefa principal que vai ser responsável por gerar as matrizes A e B, calcular esse particionamento e ao final aglomerar os resultados formando a matriz C.
*	 	
* |============= Comunicação =============|
*	No que diz respeito a comunicação, a abordagem local vs global vai ser empregada para resolver o problema. As n2 tarefas de particionamento referentes às operações linha de A x coluna de B vão ter acesso somente à linha de A e coluna de B referentes ao resultado de C que são encarregadas, que vão por sua vez ser acessadas pelas n tarefas das operações de multiplicação referentes a cada um desses resultados.
*	A tarefa principal deve comunicar-se com as tarefas de particionamento no começo do processo, para passar os dados que vão precisar para os seus cálculos, e no final para obter os resultados e gerar a matriz C.
*	As n2tarefas de particionamento não comunicam entre si durante o processo, apenas com a tarefa principal em seu início para obter a linha de A e a coluna que B relativas à sua operação, e ao final de sua execução para enviar cada uma seu resultado em C.
*	As tarefas de multiplicação tem que se preocupar ao somarem seus resultados entre si para evitar conflitos de operações de leitura-escrita simultâneas, comunicando entre si para garantir escritas uma de cada vez.
* 
* |============= Aglomeração =============|
*	Dado o nosso problema, optamos por mapear as n2 tarefas de particionamento em P processos, onde cada processo é responsável por uma mesma quantidade de tarefas, delegando mais tarefas para o último processo em casos em que não há uma divisão perfeita. Mais ainda, para cada uma das tarefas delegadas em cada processo, elas serão particionadas em n tarefas de multiplicação cada, em T threads. Cada thread vai receber uma mesma quantidade de tarefas dentre essas n, delegando mais tarefas para a última thread em casos em que não há uma divisão perfeita.
*	Portanto, dentro dos intervalos, cada thread tem a responsabilidade de calcular o valor da multiplicação entre o i-ésimo elemento da linha de A e o i-ésimo elemento da coluna de B, sendo que esses resultados parciais são serão somados e resultarão no valor final de uma das células do matriz resultado. 

* |============= Mapeamento ==============|
*	Como nesse momento só temos acesso a um dos nós do cluster por vez com o OpenMP e ele considera que todos os núcleos de processamento são homogêneos em sua distribuição de threads, nosso mapeamento é um mapeamento simples em que cada elemento de processamento receberá um processo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "omp.h"

#define N 16
#define NUMTHREADS 4

int  main(int argc, char *argv[])
{
	int npes, myrank, msgtag, src, dest, ret;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	msgtag = 1;
	
	// Calculates partitions for process
	int partitionSize = (N*N/npes);
	int start = partitionSize * myrank;
	int end = (myrank != npes - 1 ? start + partitionSize : N*N);

	if (myrank == 0)
	{
		// BTrans = transposed B
		int A[N][N], BTrans[N][N], C[N][N];

		// Create matrix
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
				// Calculates destination
				dest = (N*i + j) / partitionSize;
				if (dest > npes - 1)
					dest = npes - 1;

				// Ignores when destination is the current process 
				if (dest == 0)
					continue;

				// Sends row and column to each process calculate that part of the matrix multiplication
				MPI_Send((void*)A[i], N, MPI_INT, dest, msgtag, MPI_COMM_WORLD);
				MPI_Send((void*)BTrans[j], N, MPI_INT, dest, msgtag, MPI_COMM_WORLD);
			}
		}

		// The main process calculates the first partition
		for (int i = start; i < end - start; i++)
		{
			int row = i / N;
			int column = i % N;

			C[row][column] = 0;

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


				for(int j = start; j < end; j++)
				{
					#pragma omp atomic
						C[row][column] += A[row][j] * BTrans[column][j];
				}
			}
		}

		// Receives data from other processes and prints the result
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				// Calculates the source rank
				src = (N*i + j) / partitionSize;
				if (src > npes - 1)
					src = npes - 1;

				if (src == 0)
				{
					printf("%d ", C[i][j]);
					continue;
				}

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
		int row[N], column[N];
		int result = 0;
		
		for (int i = 0; i < end - start; i++)
		{
			result = 0;

			// Receives row and column
			MPI_Recv((void*)row, N, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);
			MPI_Recv((void*)column, N, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status);

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

				// Calculates part of the matrix multiplication using the given column and row
				for(int i = start; i < end; i++)
					#pragma omp atomic
						result += row[i] * column[i];
			}

			// Sends data back to main process
			MPI_Send((void*)&result, 1, MPI_INT, 0, msgtag, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}