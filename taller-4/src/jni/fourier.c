#include <stdio.h>
#include <omp.h>
#include <math.h>


static long max_samples = 4*100000*8;
static double x = 0;
static long initial_samples = 0;
static long sample_step = 80000;
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
	int current_samples;
	double a_, b_, sum = 0.0;
	double start_time, run_time;

	double a_0=sinh(M_PI)/M_PI;

    /*Computes e^x for each precision*/
	for (current_samples=initial_samples;current_samples<=max_samples;current_samples+=sample_step){
		sum = a_0;
		start_time = omp_get_wtime();
		int n;
		for (n=1;n<= current_samples; n++){
			double n_d = (double)n;
			a_=a_n(n);
			sum += a_*(cos(n_d*x)-n_d*sin(n_d*x));
		}
		run_time = omp_get_wtime() - start_time;
		printf("%d:%f:%f\n",current_samples, sum,run_time);
	}
}	  


