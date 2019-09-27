#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h> 

int size = 10000000;
float* x;
float* y;

void initialize(float* x, float value){
	for(int i=0;i<size;++i){
		x[i]=value;
	}
}

int main (){
	x=(float*)malloc(size*sizeof(float));
	y=(float*)malloc(size*sizeof(float));
	initialize(x, 0.1);
	initialize(y, 0.1);
	srand(time(0));

	for(int j=0;j<size;j+=1000000){
		double start_time, run_time;
		float alpha=rand();

		omp_set_num_threads(8);
		start_time = omp_get_wtime();
		int i = 0;
		#pragma omp parallel
		{
			#pragma omp for private(i)
			for(i=0;i<j;++i){
				y[i]=alpha*x[i]+y[i];
			}
		}
		
		run_time = omp_get_wtime() - start_time;

		printf("%d:%f\n",j,run_time);
	}
}	  
