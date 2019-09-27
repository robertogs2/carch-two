#include <stdio.h>
#include <omp.h>
#include <math.h>


static long num_samples = 1000000;
static double x = 0;

// returns (-1)**n
int c(int n){
	return (n&1 ? -1:1);
}

double a_n(int n){
	double n_d = (double)n;
	double result1 = 2.0*c(n)*sinh(M_PI)/(M_PI*(1+n_d*n_d));
	return result1;
}

int main (){
	int i, nprocs;
	double a_, b_, sum = 0.0;
	double start_time, run_time;

	double a_0=sinh(M_PI)/M_PI;

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
			for (n=1;n<= num_samples; n++){
				double n_d = (double)n;
				a_=a_n(n);
				sum += a_*(cos(n_d*x)-n_d*sin(n_d*x));
			}
		}

		run_time = omp_get_wtime() - start_time;
		printf("%d:%f\n",i,run_time);
	}
}	  


