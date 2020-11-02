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
	int token;

	if (myrank == 0)
	{
		token = 0;
		src = myrank;
		dest = npes > 0 ? myrank + 1 : 0;
		
		// Se for o processo de ID (myrank) = 0, ele manda o valor inicial do token (0) para o próximo processo (myrank + 1)
		MPI_Send((void*)&token, 1, MPI_INT, dest, msgtag, MPI_COMM_WORLD);

		// Daí ele aguarda para receber o token vindo do último processo na lista circular, de myrank = npes-1
		MPI_Recv((void*)&token, 1, MPI_INT, npes - 1, msgtag, MPI_COMM_WORLD, &status);
		printf("%d\n", (int) token);
	}
	else
	{
		src = myrank;

		// O processo aguarda receber o token do último processo atrás dele na fila (myrank - 1)
		MPI_Recv((void*)&token, 1, MPI_INT, src - 1, msgtag, MPI_COMM_WORLD, &status);
		
		// Daí ele envia esse token, após incrementar em 1, para o próximo na lista circular, se ele não for o último na lista
		// Se ele for o último na lista, ele envia de volta para o processo de myrank = 0
		token++;
		dest = npes != (myrank+1) ? myrank + 1 : 0;
		MPI_Send((void*)&token, 1, MPI_INT, dest, msgtag, MPI_COMM_WORLD);
	}

	fflush(0);
	MPI_Finalize();
	return 0;
}