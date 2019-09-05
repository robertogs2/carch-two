#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdint.h>
#include<stdbool.h>
#include<semaphore.h>

#define N 10

pthread_t tid[2];
uint8_t array[N];
static int _index=0;
sem_t lock;

uint8_t random_uint8(){
    return rand();
}

void* function1(void* arg){
    while(true){
		sem_wait(&lock);
        array[_index]=random_uint8();
        _index++;
        _index%=N;
        //printf("fuck\n");
        sem_post(&lock);
        usleep(1000*10);
        //printf("free\n");
    }
    return NULL;
}

void* function2(void *arg){
    while(true){
        //printf("fuck2\n");
		sem_wait(&lock);
        printf("%d ", _index);//array[_index] ^ 0x20);
        fflush(stdout);
        if(_index==0) printf("\n");
        sem_post(&lock);
        usleep(1000*10);
    }
    return NULL;

}

int main(void)
{
    int i = 0;
    int err;
    sem_init(&lock, 0, 1); 
    err = pthread_create(&(tid[0]), NULL, &function1, NULL);
    err = pthread_create(&(tid[1]), NULL, &function2, NULL);

    sleep(5);
    pthread_exit(NULL);
    return 0;
}