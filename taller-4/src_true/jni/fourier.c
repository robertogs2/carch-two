#include <stdio.h>
#include <omp.h>
#include <math.h>


static long num_samples = 1000000;
static float32_t x = 0;

// returns (-1)**n
int c(int n){
	return (n&1 ? -1:1);
}

float32_t a_n(int n){
	float32_t n_f = (float32_t)n;
	float32_t result1 = 2.0*c(n)*sinh(M_PI)/(M_PI*(1+n_d*n_d));
	return result1;
}

int main (){
	int i, nprocs;
	float32_t a_, b_, sum = 0.0;
	float32_t start_time, run_time;

	float32_t a_0=sinh(M_PI)/M_PI;

	/* Use double of system processors (threads) */
	nprocs=2*2*omp_get_num_procs();

    /*Computes e^x for each number of threads*/
	for (i=1;i<=nprocs;i++){
		sum = a_0;
		omp_set_num_threads(i);
		start_time = omp_get_wtime();
		int n;
		#pragma omp parallel  
		{
			#pragma omp for reduction(+:sum) private(a_, n)
			for (n=1;n<= num_samples;){
				float32_t n_f = (float32_t)(n++);
				float32_t a_n0 = a_n(n_f);
				float32_t cis_n0 = cos(n_f*x)-n_f*sin(n_f*x); 

				n_f = (float32_t)(n++);
				float32_t a_n1 = a_n(n_f);
				float32_t cis_n1 = cos(n_f*x)-n_f*sin(n_f*x); 

				n_f = (float32_t)(n++); 
				float32_t a_n2 = a_n(n_f); 
				float32_t cis_n2 = cos(n_f*x)-n_f*sin(n_f*x); 

				n_f = (float32_t)(n++); 
				float32_t a_n3 = a_n(n_f); 
				float32_t cis_n3 = cos(n_f*x)-n_f*sin(n_f*x); 

				float32_t vector_a[4] = {a_n0, a_n1, a_n2, a_n3};
				float32_t vector_cis[4] = {cis_n0, cis_n1, cis_n2, cis_n3};
				float32x4_t a_nv = vld1q_f32(&vector_a);
				float32x4_t cis_nv = vld1q_f32(&vector_a);

				float32x4_t rv = vmulq_f32(a_nv, cis_nv);

				float32x2_t rv = vadd_f32(vget_high_f32(rv), vget_low_f32(rv));
				float32_t res = vget_lane_f32(vpadd_f32(rv, rv), 0);
				
				sum += res;
			}
		}

		run_time = omp_get_wtime() - start_time;
		printf("%d:%f\n",i,run_time);
	}
}	  


