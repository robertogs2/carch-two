/*

This program will numerically compute the integral of

                  4/(1+x*x) 
				  
from 0 to 1. 
				 
History: Written by Tim Mattson, 11/99.

#-----------------------------------------------------

Modified by JGG 

#--------------------------------------------------------

*/

#include <stdio.h>
#include <omp.h>

long num_steps = 10000000;
double step;
int main ()
{
	

	for(long num_steps2 = num_steps; num_steps2<=num_steps*10; num_steps2+=10000000){
		int i, nprocs;
		double x, pi, sum = 0.0;
		double start_time, run_time;
		step = 1.0/(double) num_steps2;
		/*start timer */
		start_time = omp_get_wtime();
		for (i=1;i<= num_steps2; i++){
		x = (i-0.5)*step;
		sum = sum + 4.0/(1.0+x*x);
		}
		
		pi = step * sum;
		run_time = omp_get_wtime() - start_time;
		printf("%ld:%f\n",num_steps2,run_time);
	}
}	  


