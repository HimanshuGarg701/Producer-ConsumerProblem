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


int N;      //Number of Buffers
int P;      //Number of Producer Threads
int C;      //Number of consumer Threads
int X;      //Products produced by each producer
int Ptime;  //Sleep time for producer thread after producing 1 product
int Ctime;  //Sleep time for consumer thread after consuming 1 product

sem_t zeroProductsPresent;     //semaphore to check if queue is empty
sem_t allProductsPresent;      //semaphore to check if queue is full

int* productsProducedArray;      //Array for producer
int* productsConsumedArray;      //Array for Consumer

//Queue for holding all the products that producer make & from where customer comsumes the product.
int* productsQueue;
int enterIndex = 0;     //Index where product will be added.
int removeIndex = 0;    //Index where product will be removed.



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
    int product = productsQueue[removeIndex];
    productsQueue[removeIndex++] = 0;
    if(removeIndex==N){
        removeIndex = 0;
    }
    return product;
}

/* 
 * function to put item
 * into shared resource 
 * so it can be consumed
 */
void put_item(int item)
{
    productsQueue[enterIndex++] = item;
    if(enterIndex==N){
        enterIndex = 0;
    }
    return item;
}

int main(int argc, char* argv[]) 
{

    if(argc!=7){
        printf("\nMust have 6 arguments\n");
        exit(0);
    }
    else{
        N = atoi(argv[1]);
        P = atoi(argv[2]);
        C = atoi(argv[3]);
        X = atoi(argv[4]);
        Ptime = atoi(argv[5]);
        Ctime = atoi(argv[6]);

        printf("\nValues received are\n");
        printf("N is: %d\n", N);
        printf("P is: %d\n", P);
        printf("C is: %d\n", C);
        printf("X is: %d\n", X);
        printf("Ptime is: %d\n", Ptime);
        printf("Ctime is: %d\n", Ctime);

        struct timespec startTime;
        struct timespec endTime;
        time_t seconds;
        long nanoSeconds;

        clock_gettime(0, &startTime);


        clock_gettime(0, &endTime);

        seconds = endTime.tv_sec - startTime.tv_sec;
        nanoSeconds = endTime.tv_nsec - startTime.tv_nsec;
        
        if(endTime.tv_nsec < startTime.tv_nsec){
            seconds--;
            nanoSeconds = nanoSeconds + 1000000000L;
        }
        printf("\nnanoSec : %ld", nanoSeconds);
    }

}
