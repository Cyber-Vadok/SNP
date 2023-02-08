#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
int buffer = 0;
int myVar = 0;

int produce_item(){
	myVar = myVar + 2;
	return 1;
}

int consume_item(){
	myVar = myVar - 1;
	return 0;
}

void *producer() {
    while (1) {
        pthread_mutex_lock(&mutex);
		printf("\nProducer get mutex\n");
        buffer = produce_item();
		printf("Buffer %d\n",buffer);
		printf("MyVar %d\n",myVar);
        pthread_cond_signal(&cond);
		printf("Producer release mutex\n");
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void *consumer() {
    while (1) {
        pthread_mutex_lock(&mutex);
		printf("\nConsumer get mutex\n");
        while (buffer == 0) {
			printf("Consumer wait and release mutex\n");
            pthread_cond_wait(&cond, &mutex);
        }
        printf("\nConsumer get mutex after wait\n");
        buffer = consume_item();
		printf("Buffer %d\n",buffer);
		printf("MyVar %d\n",myVar);
		printf("Consumer release mutex\n");
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t producer_thread, consumer_thread;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
	pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}
