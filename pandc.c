#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>

void put_item(int item);
int grab_item();
void *consumeProduct(void *arg);
void *makeProduct(void *arg);
void checkSame();


int N;      //Number of Buffers
int P;      //Number of Producer Threads
int C;      //Number of consumer Threads
int X;      //Products produced by each producer
int Ptime;  //Sleep time for producer thread after producing 1 product
int Ctime;  //Sleep time for consumer thread after consuming 1 product
int consumption;  //Numver of items to be consumed
bool over;  //Check if items are over consumed
int extraProducts;  //Number of over produced products

sem_t zeroProductsPresent;     //semaphore to check if queue is empty
sem_t allProductsPresent;      //semaphore to check if queue is full
pthread_mutex_t block;

int* productsProducedArray;      //Array for producer
int* productsConsumedArray;      //Array for Consumer
int counterProducer = 0;             //Counter to keep track of producer array 
int counterConsumer = 0;             //Counter to keep track of consumer array 

//Queue for holding all the products that producer make & from where customer comsumes the product.
int* productsQueue;
int enterIndex = 0;     //Index where product will be added.
int removeIndex = 0;    //Index where product will be removed.

int productsCounter = 1;    //Keeps a track of number of products in queue



int main(int argc, char* argv[]) 
{
    printf("Started");
    //Need exactly 7 arguments including ./pandc
    if(argc!=7){
        printf("\nMust have 6 arguments\n");
        exit(0);
    }
    else{
        //Initializing all the arguments
        N = atoi(argv[1]);
        P = atoi(argv[2]);
        C = atoi(argv[3]);
        X = atoi(argv[4]);
        Ptime = atoi(argv[5]);
        Ctime = atoi(argv[6]);
        consumption = (P*X)/C;

        //isOver should be 0 when no extra products
        int isOver = (P*X)%C;

        //Setting boolean value for over if extra products present
        if(isOver==0){
            over = false;
        }else{
            over = true;
            //Extra products are equal to toal products produced minus total products consumed
            extraProducts = P*X - C*consumption;
        }

        //Initializing producer and consumer threads
        pthread_t producerThread[P];
        pthread_t consumerThread[C];

        //Initializing the size of consumer, product array and productsQueue
        productsQueue = (int*)calloc(N, sizeof(int));
        productsProducedArray = (int*)calloc(P*X, sizeof(int));
        productsConsumedArray = (int*)calloc(P*X, sizeof(int));


        //Printing all the arguments obtained from user
        printf("\nValues received are\n");
        printf("N is: %d\n", N);
        printf("P is: %d\n", P);
        printf("C is: %d\n", C);
        printf("X is: %d\n", X);
        printf("Ptime is: %d\n", Ptime);
        printf("Ctime is: %d\n", Ctime);
        printf("Extra Products : %d\n\n", extraProducts);


        //Noting down the start time before producing and consuming products
        struct timespec startTime;
        struct timespec endTime;
        time_t seconds;
        long nanoSeconds;

        clock_gettime(0, &startTime);

        //Initializing semaphores and pthread
        sem_init(&allProductsPresent, 0, 0);
        sem_init(&zeroProductsPresent, 0, N);
        pthread_mutex_init(&block, NULL);

        seconds = endTime.tv_sec - startTime.tv_sec;
        nanoSeconds = endTime.tv_nsec - startTime.tv_nsec;

        //Loop to create P number of producer threads
        for(int index=0; index < P; index++){
            int producerId = index+1;
            pthread_create(&producerThread[index], NULL, &makeProduct, (void*) &producerId);
        }

        //Loop to create C number of consumer threads
        for(int index=0; index<C; index++){
            int consumerId = index+1;
            pthread_create(&consumerThread[index], NULL, &consumeProduct, (void*) &consumerId);
        }

        //Loop to join all producer threads
        for(int index=0; index<P; index++){
            int producerId = index + 1;
            pthread_join(producerThread[index], NULL);
            printf("Producer joined %d\n", producerId);
        }

        //Loop to join all consumer threads
        for(int index=0; index < C;index++){
            int consumerId = index+1;
            pthread_join(consumerThread[index], NULL);
            printf("Consumer joined %d\n", consumerId);
        }

        //Destroying semaphores and pthread
        pthread_mutex_destroy(&block);
        sem_destroy(&zeroProductsPresent);
        sem_destroy(&allProductsPresent);

        clock_gettime(0, &endTime);

        if(endTime.tv_nsec < startTime.tv_nsec){
            seconds--;
            nanoSeconds = nanoSeconds + 1000000000L;
        }
        checkSame();
        printf("\nnanoSec : %ld\n", nanoSeconds);
    }
    return 0;
}

/**
 * for the 2 functions below, look
 * at the process slides/video for 
 * sample code that can be used.
 */
/* 
 * Function to remove item.
 * Item removed is returned
 */
int grab_item()
{   
    //The product removed is replaced by 0 and the product is returned
    int product = productsQueue[removeIndex];
    productsQueue[removeIndex++] = 0;
    if(removeIndex==N){
        removeIndex = 0;
    }
    return product;
}

//ConsumerProduct method deals with all the consumer threads to consume products from queue
void *consumeProduct(void *arg){
    int threadId = *((int*) arg);
    //The product that will be consumed
    int products = 0;

    int numLoops = 0;
    //Whenever there are extra products, thread has to consume consumption+extraProducts
    if(over){
        over = false;
        numLoops = consumption+extraProducts;
    }else{
        numLoops = consumption;
    }

    for(int index=0; index<numLoops; index++){
        
        //Locking the thread and semaphore to stop other threads to consume product and to
        //not consume products when queue is empty.
        sem_wait(&allProductsPresent);
        pthread_mutex_lock(&block);

        //storing the product consumed to the consumer array
        products = grab_item();
        productsConsumedArray[counterConsumer] = products;
        counterConsumer++;
        printf("%d was consumed by consumer -->\t %d\n", products, threadId);

        //Unlocking thread and semaphore
        pthread_mutex_unlock(&block);
        sem_post(&zeroProductsPresent);
        
        //Thread sleeps for Ctime after producing a product
        sleep(Ctime);
    }
    //thread exit so every thread exits after producing required products
    pthread_exit(0);
}

/* 
 * function to put item
 * into shared resource 
 * so it can be consumed
 */
void put_item(int item)
{   
    //The productsQueue gains a product whenever a product is produced
    productsQueue[enterIndex++] = item;
    if(enterIndex==N){
        enterIndex = 0;
    }
}

//Method to make the product using the producer thread
void *makeProduct(void *arg){
    int threadId = *((int*) arg);
    //The product that will be made will be stored in this variable
    int products = 0;


    //Loop will run X times, so each thread makes X products
    for(int index=0; index < X; index++){
        //ProductsCounter keeps a track of the total products
        products = productsCounter;
        productsCounter++;

        //Locking the thread and semaphore to stop other threads to produce product and to
        //not produce products when queue is full.
        sem_wait(&zeroProductsPresent);
        pthread_mutex_lock(&block);

        //Storing the product made to the consumer array
        productsProducedArray[counterProducer] = products;
        counterProducer++;
        put_item(products);

        printf("%d was produced by producer -->\t %d\n", products, threadId);

        pthread_mutex_unlock(&block);
        sem_post(&allProductsPresent);

        sleep(Ptime);
    }
    pthread_exit(0);
}

//Method to check if the consumer and producer arrays are same
void checkSame(){
    printf("\nProducer | Consumer\n");
    for(int index=0; index<P*X; index++){
        printf("%d\t | ", productsProducedArray[index]);
        printf("%d\n", productsConsumedArray[index]);
        if(productsConsumedArray[index] != productsProducedArray[index]){
            printf("Arrays don't match");
            exit(0);
        }
    }
    printf("Arrays Match");
}
