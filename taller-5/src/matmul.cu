#include <stdio.h>
#include <cuda.h>
#include <math.h>

float* Acpu;
float* Agpu;
float* Bcpu;
float* Bgpu;
float* Ccpu;
float* Ccpu2;
float* Cgpu;

//Cuda error checking - non mandatory
void cudaCheckError() {
    cudaError_t e = cudaGetLastError();
    if(e != cudaSuccess) {
        printf("Cuda failure %s:%d: '%s' : %d\n", __FILE__, __LINE__, cudaGetErrorString(e), e);
        exit(0);
    }
}

float* cpu_matrix_allocation(int rows, int cols){
	float* M = (float*) malloc(cols * rows * sizeof(float));
	return M;
}

float* gpu_matrix_allocation(float* M, int rows, int cols){
	cudaMalloc((void**) &(M), rows * cols * sizeof(float));
	return M;
}

void init_matrix(float* A, int rows, int cols){
	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			A[i*cols + j]=(i+1);
		}
	}
}

void print_matrix(float* A, int rows, int cols){
	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			printf("%f ", A[i*cols+j]);
		}
		printf("\n");
	}
	printf("\n");
}

void matmul_cpu(float* A, float* B, float* C, int rowsa, int colsa, int rowsb, int colsb) {

	if(colsa != rowsb){
    	printf("Error in matrix dimensions\n");
    	exit(0);
    }

	for(int row = 0; row < rowsa; ++row){
		for(int col = 0; col < colsb; ++col){
			float sum = 0;
	        for (int i = 0; i < colsa; i++) {
	            sum += A[row * colsa + i] * B[i * colsb + col];
	        }
	        C[row * colsb + col] = sum;
		}
	}
    
}

__global__ void matmul_kernel(float* A, float* B, float* C, int rowsa, int colsa, int rowsb, int colsb) {

    int row = blockIdx.y*blockDim.y+threadIdx.y;
    int col = blockIdx.x*blockDim.x+threadIdx.x;

    float sum = 0;

    if(colsa != rowsb){
    	printf("Error in matrix dimensions\n");
    	
    }
    else{
    	if (row < rowsa && col < colsb) {
	        // each thread computes one element of the block sub-matrix
	        for (int i = 0; i < colsa; i++) {
	            sum += A[row * colsa + i] * B[i * colsb + col];
	        }
	        C[row * colsb + col] = sum;
	    }
	    
    }
}

void check_equal(float* C, float* C2, int rows, int cols){
	for(int i = 0; i < rows*cols; ++i){
		if(C[i] != C2[i]){
			printf("Error in multiplication at row %d and column %d, data %f, %f\n", 
				i/cols ,i%cols, C[i], C2[i]);
			break;
		}
	}	
	printf("Assertion end\n");
}

void all(int rowsa, int colsa, int rowsb, int colsb, int thread_no_x, int thread_no_y){
	// Constants calculation
	int rowsc = rowsa; int colsc = colsb;
	int nbytesa = rowsa*colsa*sizeof(float);
	int nbytesb = rowsb*colsb*sizeof(float);
	int nbytesc = rowsc*colsc*sizeof(float);
	int block_no_x = (colsc + thread_no_x -1)/thread_no_x;//ceil(colsc/thread_no_x);
	int block_no_y = (rowsc + thread_no_y -1)/thread_no_y;//ceil(rowsc/thread_no_y);
	dim3 threadsPerBlock(thread_no_x, thread_no_y);
    dim3 blocksPerGrid(block_no_x, block_no_y);

	printf("Begin\n");

	//Allocation
	Acpu = cpu_matrix_allocation(rowsa, colsa);
	Bcpu = cpu_matrix_allocation(rowsb, colsb);
	Ccpu = cpu_matrix_allocation(rowsc, colsc);
	Ccpu2 = cpu_matrix_allocation(rowsc, colsc);
	printf("Allocated cpu\n");
	Agpu = gpu_matrix_allocation(Agpu, rowsa, colsa);
	Bgpu = gpu_matrix_allocation(Bgpu, rowsb, colsb);
	Cgpu = gpu_matrix_allocation(Cgpu, rowsc, colsc);
	printf("Allocated gpu\n");

	// Initializes matrices
	init_matrix(Acpu, rowsa, colsa);
	init_matrix(Bcpu, rowsb, colsb);
	cudaMemcpy(Agpu, Acpu, nbytesa, cudaMemcpyHostToDevice);
	cudaMemcpy(Bgpu, Bcpu, nbytesb, cudaMemcpyHostToDevice);
	printf("Initialized matrices\n");

	// Multiply cpu
	clock_t start = clock();
	matmul_cpu(Acpu, Bcpu, Ccpu, rowsa, colsa, rowsb, colsb);
	clock_t time_cpu = clock()-start;
	printf("Multiplied cpu\n");

	// Multiply gpu
	start = clock();
    matmul_kernel<<<blocksPerGrid, threadsPerBlock>>>(Agpu, Bgpu, Cgpu, rowsa, colsa, rowsb, colsb);
    cudaDeviceSynchronize();
    clock_t time_gpu = clock()-start;
    printf("Multiplied gpu\n");

    // rowsc, colsc, b_x, b_y, gpu time, cpu time
	printf("%d,%d,%d,%d,%f,%f\n", 
		rowsc, colsc, block_no_x,block_no_y,(double)time_gpu/CLOCKS_PER_SEC,(double)time_cpu/CLOCKS_PER_SEC);
    cudaMemcpy(Ccpu2, Cgpu, nbytesc, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    //print_matrix(Ccpu2, rowsc, colsc);
    //print_matrix(Ccpu, rowsc, colsc);
    check_equal(Ccpu, Ccpu2, rowsc, colsc);
}

int main(int argc, char** argv){

	// Variables
	int rowsa = 1000; int colsa = 445;
	int rowsb = 445; int colsb = 1000;
	int thread_no_x = 100; int thread_no_y = 10; // tx * ty < 1024!

	all(rowsa, colsa, rowsb, colsb, thread_no_x, thread_no_y);

}