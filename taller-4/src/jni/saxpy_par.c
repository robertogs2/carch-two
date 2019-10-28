#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h> 
#include <arm_neon.h>

#define vectors 10000000
#define float_amount 4*vectors
float32_t* x;
float32_t* y;

void initialize(float32_t* x, int size){
	for(int i=0;i<size;++i){
		x[i]=(float)5*(rand()%10);
	}
	printf("\n");
}

void printvec(float32_t* x, int size){
	for(int i=0;i<size;++i){
		printf("%f ", x[i]);
	}
	printf("\n");
}

int main (){
	x=(float32_t*)malloc(float_amount*sizeof(float32_t));
	y=(float32_t*)malloc(float_amount*sizeof(float32_t));
	srand(time(0));
	initialize(x, (int)float_amount);
	initialize(y, (int)float_amount);
	float32_t a = 3.0;
	float32x4_t av = vmovq_n_f32(a); 				// Replicates the constant
	//printvec(x, float_amount); printvec(y, float_amount);
	for(int j=0;j<=float_amount;j+=1000000){
		double start_time, run_time;

		start_time = omp_get_wtime();
		int i = 0;
		#pragma omp parallel
		{
			#pragma omp for private(i)
			for(i=0;i<float_amount;i+=4){
				float32x4_t yv = vld1q_f32(y+i); 	// yv = *(y+i)
				float32x4_t xv = vld1q_f32(x+i); 	// xv = *(x+i)
				yv = vmlaq_f32(yv, xv, av); 		// yv = av * xv + yv
				vst1q_f32(y+i, yv);					// *(y+i) = yv
			}
		}
		
		run_time = omp_get_wtime() - start_time;
		printf("%d:%f\n",j,run_time);
	}
	//printvec(x, float_amount); printvec(y, float_amount);
	
}	  
