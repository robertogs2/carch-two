#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <arm_neon.h>

static long max_samples = 4*100000*8;
static long initial_samples = 0;
static long sample_step = 80000;
static float32_t x = 0;

// returns (-1)**n
int c(int n){
	return (n&1 ? -1:1);
}

float32_t a_n(int n){
	float32_t n_f = (float32_t)n;
	float32_t result1 = 2.0*c(n)*sinh(M_PI)/(M_PI*(1+n_f*n_f));
	return result1;
}

int main (){
	int i, nprocs;
	float32_t sum = 0.0;
	double start_time, run_time;

	float32_t a_0=sinh(M_PI)/M_PI;
	int current_samples;
    /*Computes e^x for each precision*/
	for (current_samples=initial_samples;current_samples<=max_samples;current_samples+=sample_step){
		sum = a_0;
		start_time = omp_get_wtime();
		int n;
		#pragma omp parallel  
		{
			#pragma omp for reduction(+:sum) private(n)
			for (n=1;n<= current_samples;n+=4){
				// Calculates each value first

				//First value
				float32_t n_f = (float32_t)(n);
				float32_t a_n0 = a_n(n_f);
				float32_t cis_n0 = cos(n_f*x)-n_f*sin(n_f*x); 

				//Second value
				n_f = (float32_t)(n+1);
				float32_t a_n1 = a_n(n_f);
				float32_t cis_n1 = cos(n_f*x)-n_f*sin(n_f*x); 

				//Third value
				n_f = (float32_t)(n+2); 
				float32_t a_n2 = a_n(n_f); 
				float32_t cis_n2 = cos(n_f*x)-n_f*sin(n_f*x); 

				//Fourth value
				n_f = (float32_t)(n+3); 
				float32_t a_n3 = a_n(n_f); 
				float32_t cis_n3 = cos(n_f*x)-n_f*sin(n_f*x); 

				// Creates vectors
				float32_t vector_a[4] = {a_n0, a_n1, a_n2, a_n3};
				float32_t vector_cis[4] = {cis_n0, cis_n1, cis_n2, cis_n3};
				float32x4_t a_nv = vld1q_f32(vector_a);
				float32x4_t cis_nv = vld1q_f32(vector_cis);

				float32x4_t rv = vmulq_f32(a_nv, cis_nv); 							// Multiplies the values 
				float32x2_t rv2 = vadd_f32(vget_high_f32(rv), vget_low_f32(rv)); 	// Sums the values
				float32_t res = vget_lane_f32(vpadd_f32(rv2, rv2), 0); 				
				
				sum += res;
			}
		}

		run_time = omp_get_wtime() - start_time;
		printf("%d:%f:%f\n",current_samples, sum,run_time);
	}
}	  


