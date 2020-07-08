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
    
}

/* 
 * function to put item
 * into shared resource 
 * so it can be consumed
 */
void put_item(int item)
{
    
}

int main(int argc. char* argv[]) 
{
    struct timespec startTime;
    struct timespec endTime;
    time_t seconds;
    long nanoSeconds;

    startTime = clock_gettime(CLOCK_REALTIME, &start_time);
    endTime = clock_gettime(CLOCK_REALTIME, &end_time);

    seconds = end_time.tv_sec - start_time.tv_sec;
    nano_seconds = end_time.tv_nsec - start_time.tv_nsec;
    
    if(end_time.tv_nsec < start_time.tv_nsec){
        seconds--;
        nano_seconds = nano_seconds + 1000000000L;
    }

}
