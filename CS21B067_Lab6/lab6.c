#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

int N=0, P=0, S=0, C=0; 
int *buffer;
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
int bound = 0;

pthread_cond_t empty = PTHREAD_COND_INITIALIZER, fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % S;
    count++;
}

int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % S;
    count--;
    return tmp;
}

void *producer(void *arg) {
    int i;
    int ID = *((int *)arg);
    int randomVal;
    for (i = 0; i < N; i++) {
        randomVal = (rand()%(9000)) + 1000;
        pthread_mutex_lock(&mutex);
        while (count == S)
        {
            pthread_cond_wait(&empty, &mutex); 
        }
        put(randomVal);
        printf("Producer Thread #: %d; Item #: %d; Added Number: %d\n", ID, i+1, randomVal);
        sleep((rand()%3));
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) { 
    int i;
    int ID = *((int *)arg);
    for (i = 0; i < bound; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0)
        {
            pthread_cond_wait(&fill, &mutex);
        }
        int tmp = get();
        printf("Consumer Thread #: %d; Item #: %d; Read Number: %d\n", ID, i+1, tmp);
        sleep(rand()%4);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char * argv[])
{
    for(int i=1; i<argc; i++)
    {
        if(strcmp("-p", argv[i-1])==0)
        {
            P = atoi(argv[i]);
        }  
        if(strcmp("-n", argv[i-1])==0)
        {
            N = atoi(argv[i]);
        } 
        if(strcmp("-s", argv[i-1])==0)
        {
            S = atoi(argv[i]);
        } 
        if(strcmp("-c", argv[i-1])==0)
        {
            C = atoi(argv[i]);
        }  
    }
    assert(C!=0);
    buffer = (int *)malloc(S*(sizeof(int)));
    bound = (0.7*N*P)/C;
    pthread_t producerThreads[P], consumerThreads[C];
    int producerIDs[P], consumerIDs[C];
    int i;
    for(int j=0; j<P; j++)
    {
        producerIDs[j] = j+1;
    }
    for(int j=0; j<C; j++)
    {
        consumerIDs[j] = j+1;
    }
    for(i=0; i<P; i++)
    {
        pthread_create(&producerThreads[i], NULL, producer, (void *)&producerIDs[i]);
    }
    for(i=0; i<C; i++)
    {
        pthread_create(&consumerThreads[i], NULL, consumer, (void *)&consumerIDs[i]);
    }
    for(i=0; i<P; i++)
    {
        pthread_join(producerThreads[i], NULL);
    }
    for(i=0; i<C; i++)
    {
        pthread_join(consumerThreads[i], NULL);
    }
    printf("Program over\n\n");
    free(buffer);
    return 0;
}