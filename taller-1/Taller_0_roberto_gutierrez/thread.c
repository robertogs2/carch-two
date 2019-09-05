//Example for sharing variables betweem threads and syncing them
//Sync the index of an array and a sampling condition

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdint.h>
#include<stdbool.h>
#include<time.h> 
#include<math.h>

#define N 10

pthread_t threads[2];
uint8_t array[N];

static int _index=0;
static bool sampled=false;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

uint8_t random_uint8(){
    return rand();
}

void* sampler(void* arg){
    while(true){
        pthread_mutex_lock(&lock);
        if(!sampled){
            usleep(1000*10);
            array[_index]=random_uint8();
            if(++_index==N) _index=0;
            sampled=true;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* printer(void *arg){
    while(true){
        pthread_mutex_lock(&lock);
        if(sampled){
            if(!_index){ // Fix cause index in one position ahead
                printf("%c\n", array[N-1] ^ 0x20);
            }
            else printf("%c\n", array[_index-1]);
            sampled=false;
            
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void)
{
    int i = 0;
    int err;
    srand(time(0)); 

    err = pthread_create(&(threads[0]), NULL, &sampler, NULL);
    err = pthread_create(&(threads[1]), NULL, &printer, NULL);

    sleep(5);
    pthread_exit(NULL);
    return 0;
}