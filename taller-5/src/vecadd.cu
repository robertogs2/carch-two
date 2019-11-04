#include <stdio.h>
#include <cuda.h>
#include <math.h>

int *a, *b;  // host data
int *c, *c2;  // results

//Cuda error checking - non mandatory
void cudaCheckError() {
    cudaError_t e = cudaGetLastError();
    if(e != cudaSuccess) {
        printf("Cuda failure %s:%d: '%s' : %d\n", __FILE__, __LINE__, cudaGetErrorString(e), e);
        //exit(0);
    }
}

//GPU kernel
__global__
void vecAdd(int *A, int *B, int *C, int N) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    C[i] = A[i] + B[i];
}

//CPU function
void vecAdd_h(int *A1, int *B1, int *C1, int N) {
    for(int i = 0; i < N; i++)
        C1[i] = A1[i] + B1[i];
}

void print_vec(int *vector, int n) {
    for(int i = 0; i < n; ++i) {
        printf("%d ", vector[i]);
    }
    printf("%s\n", "");
}

void all(int n, int block_size) {
    //Number of blocks
    int nBytes = n * sizeof(int);
    //Block size and number
    int block_no;
    block_no = ceil(n / block_size);

    if (block_no == 0) block_no = 1;

    //memory allocation
    a = (int *) malloc(nBytes);
    b = (int *) malloc(nBytes);
    c = (int *) malloc(nBytes);
    c2 = (int *) malloc(nBytes);
    cudaCheckError();
    int *a_d, *b_d, *c_d;


    //Work definition
    dim3 dimBlock(block_size, 1, 1);
    dim3 dimGrid(block_no, 1, 1);

    // Data filling
    for(int i = 0; i < n; i++)
        a[i] = i, b[i] = i;


    //printf("Allocating device memory on host..\n");
    //GPU memory allocation
    cudaMalloc((void **) &a_d, n * sizeof(int));
    cudaMalloc((void **) &b_d, n * sizeof(int));
    cudaMalloc((void **) &c_d, n * sizeof(int));

    //printf("Copying to device..\n");
    cudaMemcpy(a_d, a, n * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(b_d, b, n * sizeof(int), cudaMemcpyHostToDevice);

    clock_t start_d = clock();
    //printf("Doing GPU Vector add\n");
    vecAdd <<< block_no, block_size>>>(a_d, b_d, c_d, n);
    cudaCheckError();

    //Wait for kernel call to finish
    cudaDeviceSynchronize();

    clock_t end_d = clock();


    //printf("Doing CPU Vector add\n");
    clock_t start_h = clock();
    vecAdd_h(a, b, c2, n);
    clock_t end_h = clock();

    //Time computing
    double time_d = (double)(end_d - start_d) / CLOCKS_PER_SEC;
    double time_h = (double)(end_h - start_h) / CLOCKS_PER_SEC;

    //Copying data back to host, this is a blocking call and will not start until all kernels are finished
    cudaMemcpy(c, c_d, n * sizeof(int), cudaMemcpyDeviceToHost);
    // n, b size, b number, gpu time, cpu time
    printf("%d,%d,%d,%f,%f\n", n,block_size,block_no,time_d,time_h);

    // print_vec(a, n);
    // print_vec(b, n);
    // print_vec(c, n);

    //Free GPU memory
    cudaFree(a_d);
    cudaFree(b_d);
    cudaFree(c_d);
}

int main(int argc, char **argv) {

    printf("Begin \n");

    for(int i = 0; i < 10; ++i){
        for(int j = 1; j < 10; ++j){
            //Iterations
            int n = 100000*i+100;
            int block_size = 100*j; //threads per block
            //printf("Using n=%d and b=%d\n", n, block_size);
            all(n, block_size);
        }
    }
    

    return 0;
}