#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <string.h>

#define MAX_STR_SIZE 256
void* baker(void* args);
int getUserBakers(char* str);
int main()
{
    pthread_t thread;
    char userString[MAX_STR_SIZE];
    char* endPtr;
    int numBakers = -1;
    printf("******* The Great Bake Off ***********\n");
    do{
        printf("Please enter the number of bakers for the bake off: ");
        numBakers = getUserBakers(userString);
    }while(numBakers == -1);

    for(int i =0; i <= numBakers; i++)
    {
        pthread_create(&thread, NULL, baker, (void*) &i);
        pthread_detach(thread);
    }
    return 0;
}

int getUserBakers(char* str)
{
    char* endPtr;
    fgets(str, MAX_STR_SIZE, stdin);
    str = strtok(str, "\n");
    int bakers = strtoul(str, &endPtr, 10);
    if(*endPtr == '\0' && bakers > 0)
        return bakers;
    else
        printf("Please enter a valid decimal number for bakers\n");
        return -1;
}

void* baker(void* args)
{
    int threadID = 0;
    memcpy(&threadID, (int*)args, sizeof(int));
    printf("hello from thread %d\n", threadID);
    pthread_exit((void*) 0);
}