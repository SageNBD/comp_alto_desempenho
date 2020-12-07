/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

/*
Soma dois vetores
Ilustra a alocação dinâmica da memoria compartilhada
*/

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <limits.h>

#define TAM 16
#define TPB 2

__global__ void min_max(int *vetorA, int *vetorB,int *min_max_val,int tam)
{ 
    int idx = blockDim.x * blockIdx.x + threadIdx.x; 
    if (idx < tam) 
    {
        if (vetorA[idx] < min_max_val[0])
            min_max_val[0] = vetorA[idx];
        if (vetorA[idx] > min_max_val[1])
            min_max_val[1] = vetorA[idx];
        if (vetorB[idx] < min_max_val[0])
            min_max_val[0] = vetorB[idx];
        if (vetorB[idx] > min_max_val[1])
            min_max_val[1] = vetorB[idx];
    }
}

__global__ void mult(int *vetorA, int *vetorB,int *vetorC,int tam)
{ 
    int idx = blockDim.x * blockIdx.x + threadIdx.x; 
    if (idx < tam) 
    {
        vetorC[idx] = vetorA[idx] * vetorB[idx];
    }
}

// __global__ void soma(int *vetorA, int *vetorB,int *vetorC,int tam)
// { 
//     int idx = blockDim.x * blockIdx.x + threadIdx.x; 
//     if (idx < tam) 
//     {
//         vetorC[idx]=vetorA[idx]+vetorB[idx];
//     }
// }

int main(int argc,char **argv){

    //Declara as matrizes
    int *A,*B,*C;
    //Declara as variáveis de índice
    int i,j,k,dim;
    //Declara o acumulador para o produto escalar global
    int somape, minimo = INT_MAX, maximo = INT_MIN;
    int* min_max_val;

    //Declara um vetor para os produtos escalares locais
    // int prod_escalar;

    FILE *inputfile;     // handler para o arquivo de entrada
    char *inputfilename;  // nome do arquivo de entrada

   
    if (argc < 2)
    {
        printf("Please run with input file name, i.e., num_perf_mpi inputfile.ext\n");
        exit(-1);
    }

    inputfilename = (char*) malloc (256*sizeof(char));
    strcpy(inputfilename,argv[1]);

	printf("inputfilename=%s\n", inputfilename);
	fflush(0);

    if ((inputfile=fopen(inputfilename,"r")) == 0)
    {
		printf("Error openning input file.\n");
		exit(-1);
    }
    
    fscanf(inputfile, "%d\n", &dim); //Lê a dimensão das matrizes
    
    //Aloca as matrizes
    cudaHostAlloc((void**)&A, dim * dim * sizeof(int), cudaHostAllocMapped);
    cudaHostAlloc((void**)&B, dim * dim * sizeof(int), cudaHostAllocMapped); 
    cudaHostAlloc((void**)&C, dim * dim * sizeof(int), cudaHostAllocMapped); 
    cudaHostAlloc((void**)&min_max_val, 3 * sizeof(int), cudaHostAllocMapped); 

    //Aloca um vetor para armazenar os produtos escalares de cada linha
    // cudaHostAlloc((void*)&prod_escalar, sizeof(int), cudaHostAllocMapped);

     //Lê a matriz A
    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            fscanf(inputfile, "%d ",&(A[i*dim+j]));


    //Lê a matriz B
    for (i = 0; i < dim; i++)
        for(j = 0; j < dim; j++)
            fscanf(inputfile, "%d ",&(B[i*dim+j])); 

    min_max_val[0] = INT_MAX;
    min_max_val[1] = INT_MIN;

	cudaDeviceSynchronize();
	
	// fecha o arquivo de entrada
	fclose(inputfile);





    int threadsPerBlock,blocksPerGrid; 

    //Define a quantidade de threads por bloco
    threadsPerBlock = 256; 
    
    //Define a quantidade de blocos por grade
    blocksPerGrid = (dim * dim + threadsPerBlock - 1) / threadsPerBlock; 




    //Invoca o kernel com blocksPerGrid blocos e threadsPerBlock threads
    mult <<<blocksPerGrid, threadsPerBlock>>> (A, B, C, dim * dim); 
    min_max <<<blocksPerGrid, threadsPerBlock>>> (A, B, min_max_val, dim * dim); 

	cudaDeviceSynchronize();
	
    //Imprime o resultado no host
    int result = 0;
    for(i = 0; i < dim * dim; i++)
    {
        // printf("%d ",C[i]);
        result += C[i];
    }
    
    printf("%d\n", result);

    printf("%d %d", min_max_val[0], min_max_val[1]);
	
	

    //Desaloca os vetores no host e no device
    cudaFreeHost(A);
    cudaFreeHost(B);
    cudaFreeHost(C);
    cudaFreeHost(min_max_val);

}