#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <string.h>

#define MAX_STR_SIZE 256
int getUserBakers(char* str);
int main()
{
    char userString[MAX_STR_SIZE];
    char* endPtr;
    int numBakers = -1;
    printf("******* The Great Bake Off ***********\n");
    do{
        printf("Please enter the number of bakers for the bake off: ");
        numBakers = getUserBakers(userString);
    }while(numBakers == -1);

    printf("Num bakers is %d\n", numBakers);
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

}