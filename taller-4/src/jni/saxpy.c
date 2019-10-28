#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define vectors 10000000
#define float_amount 4*vectors

float* x;
float* y;

void initialize(float* x, float value){
	for(int i=0;i<float_amount;++i){
		x[i]=value;
	}
}

int main (){
	x=(float*)malloc(float_amount*sizeof(float));
	y=(float*)malloc(float_amount*sizeof(float));
	initialize(x, 0.1);
	initialize(y, 0.1);
	for(int j=0;j<=float_amount;j+=1000000){
		double start_time, run_time;
		float alpha=6.4;

		start_time = omp_get_wtime();
		for(int i=0;i<j;++i){
			y[i]=alpha*x[i]+y[i];
		}
		run_time = omp_get_wtime() - start_time;

		printf("%d:%f\n",j,run_time);
	}
	
}	  