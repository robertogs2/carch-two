// SSE headers
#include <emmintrin.h> //v3
#include <smmintrin.h> //v4

#include <stdio.h>
#include <string.h>

typedef struct{
	__m128i rows[2];
} matrix2x8;

typedef struct{
	__m128 rows[4];
} matrix4x4;

void print_vector(__m128 vector){
	float* row_p = (float*) &vector;
	for(int j = 0; j < 4; ++j){
		printf("%f ", row_p[j]);
	}
	printf("\n");
}

void print_matrix(matrix4x4 m){
	for(int i = 0; i < 4; ++i){
		__m128 row = m.rows[i];
		print_vector(row);
	}
}

__m128i max_columns(matrix2x8 m){
	return _mm_max_epi16(m.rows[0], m.rows[1]);
}

matrix4x4 multiply(matrix4x4 m0, matrix4x4 m1){
	matrix4x4 mret;
	for(int i = 0; i < 4; ++i){
		float* row = (float*) &m0.rows[i];
		__m128 result_row = _mm_setzero_ps();
		for(int j = 0; j < 4; ++j){
			__m128 scales = _mm_set1_ps(row[j]);
			__m128 mult = _mm_mul_ps(scales, m1.rows[j]);
			//printf("%f\n", row[j]);
			result_row = _mm_add_ps(result_row, mult);
		}
		mret.rows[i]=result_row;
	}
	return mret;
}

void compute_max(matrix2x8 m1){
	__m128i result = max_columns(m1);
	short* r = (short*) &result;
	for(int i = 0; i < 8; ++i){
		printf("%hu ", r[i]);
	}
	printf("\n");
}

int test(){
	//****Nota: este código requiere SSE4****//
	printf("Hola Mundo desde SSE \n");
	matrix2x8 m1;
	m1.rows[0] = _mm_set_epi16(2, 2, 2, 4, 7, 7, 8, 9);
	m1.rows[1] = _mm_set_epi16(4, 3, 2, 1, 0, 0, 0, 0);

	matrix4x4 m2;
	matrix4x4 m3;

	for(int i = 0; i<4; ++i){
		m2.rows[i] = _mm_set_ps(1,1,1,1);
		m3.rows[i] = _mm_setzero_ps();

		float* row_p = (float*) &m3.rows[i];
		row_p[i]=i;
	}

	matrix4x4 m4 = multiply(m2, m3);
	
	printf("Hola Mundo desde SSE \n");

	print_matrix(m4);

	compute_max(m1);
	
	return 1;
}

int processMaximun(){
	matrix2x8 m;
	printf("Please insert 8 16 bit numbers for first row separated by something\n");
	short* r  = (short*) &m.rows[0];
	scanf("%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu", &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7]);
	r  = (short*) &m.rows[1];
	printf("Please insert 8 16 bit numbers for second row separated by something\n");
	scanf("%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu%*c%hu", &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7]);
	printf("Result is: \n");
	compute_max(m);
}

int processMultiplication(){
	matrix4x4 m1;
	matrix4x4 m2;
	printf("Please insert first matrix separated by spaces or newlines\n");
	float* f = (float*) &m1.rows[0];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m1.rows[1];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m1.rows[2];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m1.rows[3];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	printf("Please insert second matrix separated by spaces or newlines\n");
	f = (float*) &m2.rows[0];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m2.rows[1];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m2.rows[2];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);
	f = (float*) &m2.rows[3];
	scanf("%f%*c%f%*c%f%*c%f%*c", &f[0], &f[1], &f[2], &f[3]);

	matrix4x4 result = multiply(m1, m2);
	printf("Result:\n");
	print_matrix(result);
	printf("\n");

}


int main(){
	char c;
	while(1){ // Menu loop
		printf("Please enter 1 for max or 2 for matrix multiplication\n");
	   	c = getc(stdin);
   		int number = c-'0';
   		if(number == 1){
   			processMaximun();	
   		}
   		else if(number == 2){
   			processMultiplication();
   		}
   		else{
   			printf("Number is wrong\n");
   		}
	}
	return 1;
}