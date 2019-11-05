#include <stdio.h>
#include <cuda.h>
#include <math.h>

float* Ycpu;
float* Ygpu;
float* Xcpu;
float* Xgpu;

//Cuda error checking - non mandatory
void cudaCheckError() {
    cudaError_t e = cudaGetLastError();
    if(e != cudaSuccess) {
        printf("Cuda failure %s:%d: '%s' : %d\n", __FILE__, __LINE__, cudaGetErrorString(e), e);
        exit(0);
    }
}

float* cpu_allocation(int n){
	return (float*) malloc(n * sizeof(float));
}

float* gpu_allocation(float* vector, int n){
	cudaMalloc((void**) &(vector), n * sizeof(float));
	return vector;
}

void init_vector(float* vector, int n){
	for(int i = 0; i < n; ++i){
		vector[i]=(i+1);
	}
}

void print_vec(float* vector, int n){
	for(int i = 0; i < n; ++i){
		printf("%f ", vector[i]);
	}
	printf("\n");
}

void saxpy_cpu(float* y, float* x, float a, int n) {

	for(int i = 0; i < n; ++i){
		y[i] = a*x[i]+y[i];
	}
    
}

__global__ void saxpy_kernel(float* y, float* x, float a, int n) {

    int index = blockIdx.x*blockDim.x+threadIdx.x;

    if(index < n) y[index] = a*x[index]+y[index];
}

void check_equal(float* v1, float* v2, int n){
	for(int i = 0; i < n; ++i){
		if(v1[i] != v2[i]){
			printf("Error at index %d", i);
			break;
		}
	}	
}

void all(int n, int thread_no){
	// Constants calculation
	int nbytes = n*sizeof(float);
	int block_no = (n + thread_no -1)/thread_no;//ceil(colsc/thread_no);
	dim3 threadsPerBlock(thread_no, 1);
    dim3 blocksPerGrid(block_no, 1);

	//Allocation
	Ycpu = cpu_allocation(n);
	Xcpu = cpu_allocation(n);
	Xgpu = gpu_allocation(Xgpu, n);
	Ygpu = gpu_allocation(Ygpu, n);

	// Initializes matrices
	init_vector(Ycpu, n);
	init_vector(Xcpu, n);
	cudaMemcpy(Ygpu, Ycpu, nbytes, cudaMemcpyHostToDevice);
	cudaMemcpy(Xgpu, Xcpu, nbytes, cudaMemcpyHostToDevice);

	// Saxpy cpu
	clock_t start = clock();
	saxpy_cpu(Ycpu, Xcpu, 2, n);
	clock_t time_cpu = clock()-start;

	// Saxpy gpu
	start = clock();
    saxpy_kernel<<<blocksPerGrid, threadsPerBlock>>>(Ygpu, Xgpu, 2, n);
    cudaDeviceSynchronize();
    clock_t time_gpu = clock()-start;
  

    // n, blocks, gpu time, cpu time
	printf("%d,%d,%f,%f\n", 
		n, block_no,(double)time_gpu/CLOCKS_PER_SEC,(double)time_cpu/CLOCKS_PER_SEC);
    //cudaMemcpy(Ccpu2, Cgpu, nbytesc, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    //print_vector(Ccpu2, rowsc, colsc);
    //print_vector(Ccpu, rowsc, colsc);
    //check_equal(Xcpu, Ccpu2, rowsc, colsc);
}

int main(int argc, char** argv){

	int thread_no = 100;
	// Variables
	for(int size = 4; size <= 10000; size+=100){
		all(size, thread_no);
	}

}
