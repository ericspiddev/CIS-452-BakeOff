#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_STR_SIZE 256
void* baker(void* args);
int getUserBakers(char* str);

//we could create structures that contain ingredients for each recipe

struct sembuf mixer;
struct sembuf pantry;
struct sembuf fridge;
struct sembuf bowl;
struct sembuf spoon;
struct sembuf oven;
int mixerID, pantryID, fridgeID, bowlID, spoonID, ovenID;

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

void initializeSemaphores(void){
    mixerID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (mixerID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(mixerID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    mixer.sem_num = 0;
    mixer.sem_op = 2;
    mixer.sem_flg = 0;
    semop(mixerID, &mixer, 1);

    pantryID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (pantryID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(pantryID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    pantry.sem_num = 0;
    pantry.sem_op = 1;
    pantry.sem_flg = 0;
    semop(pantryID, &pantry, 1);

    fridgeID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (fridgeID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(fridgeID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    fridge.sem_num = 0;
    fridge.sem_op = 2;
    fridge.sem_flg = 0;
    semop(fridgeID, &fridge, 1);

    bowlID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (bowlID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(bowlID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    bowl.sem_num = 0;
    bowl.sem_op = 3;
    bowl.sem_flg = 0;
    semop(bowlID, &bowl, 1);

    spoonID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (spoonID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(spoonID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    spoon.sem_num = 0;
    spoon.sem_op = 5;
    spoon.sem_flg = 0;
    semop(spoonID, &spoon, 1);

    ovenID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (ovenID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(ovenID, 0, SETVAL, 1) < 0){
        perror("Failed to control resource");
        exit(1);
    }
    oven.sem_num = 0;
    oven.sem_op = 1;
    oven.sem_flg = 0;
    semop(ovenID, &oven, 1); 
}