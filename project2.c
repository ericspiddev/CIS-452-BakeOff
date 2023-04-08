#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_STR_SIZE 256
#define NUMRECIPES 5

#define WAIT -1;
#define SIGNAL 1;


typedef enum recipeNames {
    COOKIES,
    PANCAKES,
    PIZZA,
    PRETZELS,
    CINNAMONROLLS
}recipeNames;

typedef enum ingredients{
    FLOUR,
    SUGAR,
    YEAST,
    BAKINGSODA,
    SALT,
    CINNAMON,
    EGGS,
    MILK,
    BUTTER
} ingredients;

struct sembuf mixer;
struct sembuf pantry;
struct sembuf fridge;
struct sembuf bowl;
struct sembuf spoon;
struct sembuf oven;
int mixerID, pantryID, fridgeID, bowlID, spoonID, ovenID;

typedef struct recipe {
    ingredients* recipeIngredients;
    bool* currentIngredients;
    int numIngredients;
    bool isMixed;
    bool isBaked;
} recipe;

//function prototypes
void* baker(void* args);
int getUserBakers(char* str);
bool hasAllIngredients(recipe r);
void initIngredients(recipe* r, recipeNames name);
bool isInFridge(ingredients ingredient);
bool isInPantry(ingredients ingredient);
bool isBakerDone(recipe* bakersRecipeList);
recipeNames intToRecipeNames(int index);
int assignNextRecipe(recipe* list);
void initializeSemaphores(void);
void getIngredientsFromFridge(int baker, recipe* rlist, recipeNames name);
void getIngredientsFromPantry(int bakerID, recipe* rlist, recipeNames name);
void mixIngredients(int bakerId, recipe* r, recipeNames name);
void bakeRecipe(int bakerId, recipe* r, recipeNames name);
void recipeInit(void);
char* recipeNamesToString(recipeNames recName);


recipe recipeList[NUMRECIPES];

int main()
{
    char userString[MAX_STR_SIZE];
    char* endPtr;
    int numBakers = -1;
    initializeSemaphores();
    recipeInit();
    printf("******* The Great Bake Off ***********\n");
    do{
        printf("Please enter the number of bakers for the bake off: ");
        numBakers = getUserBakers(userString);
    }while(numBakers == -1);

    pthread_t threads[numBakers];
    for(int i =0; i < numBakers; i++)
    {
        pthread_create(&threads[i], NULL, baker, (void*) i);
    }
    for(int i =0; i < numBakers; i++)
    {
        pthread_join(threads[i], NULL);
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
    int bakerId = args + 1;
    // memcpy(&bakerId, (int*)args, sizeof(int));
    printf("baker id is %d\n", bakerId);
    int recipeIndex = bakerId % NUMRECIPES;

    recipe* bakersRecipeList = malloc(sizeof(recipeList));
    if(bakersRecipeList < 0){
        perror("Could not allocate memory to copy over recipe list");
    }
    memcpy(bakersRecipeList, recipeList, sizeof(recipeList));
    recipe currRecipe = bakersRecipeList[recipeIndex];
    while(!isBakerDone(bakersRecipeList))
    {
        int ingredientCount = 0;
        while(!hasAllIngredients(bakersRecipeList[recipeIndex])) // should have them all after one loop but just in case
        {
            getIngredientsFromFridge(bakerId, bakersRecipeList, intToRecipeNames(recipeIndex));
            getIngredientsFromPantry(bakerId, bakersRecipeList, intToRecipeNames(recipeIndex));
        }
        // grab bowl, mixer, spoon
        mixIngredients(bakerId, &bakersRecipeList[recipeIndex], intToRecipeNames(recipeIndex));
        // put in the oven
        bakeRecipe(bakerId, &bakersRecipeList[recipeIndex], intToRecipeNames(recipeIndex));
        // mark as done and update index
        recipeIndex = assignNextRecipe(bakersRecipeList);
    }
    printf("-=+=-=+=-=+-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-\n");
    printf("Baker %d has finished all the recipes!\n", bakerId);
    printf("-=+=-=+=-=+-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-=+=-\n");
    free(bakersRecipeList);
    pthread_exit((void*) 0);
}

recipeNames intToRecipeNames(int index)
{
    switch(index)
    {
        case 0:
            return COOKIES;
        case 1:
            return PANCAKES;
        case 2:
            return PIZZA;
        case 3:
            return PRETZELS;
        case 4:
            return CINNAMONROLLS;
        default:
            printf("Out of Bounds RecipeName index!\n");
            exit(-1);
    }
}

char* recipeNamesToString(recipeNames recName)
{
    switch(recName)
    {
        case COOKIES:
            return "COOKIES";
        case PANCAKES:
            return "PANCAKES";
        case PIZZA:
            return "PIZZA";
        case PRETZELS:
            return "PRETZELS";
        case CINNAMONROLLS:
            return "CINNAMON ROLLS";
        default:
            return "IDK THIS RECIPE";
    }
}

int assignNextRecipe(recipe* list)
{
    for(int i = 0; i < NUMRECIPES; i++)
    {
        if(list[i].isBaked == true)
            continue;
        return i;
    }
    return -1;
}

char* ingredientNamesToString(ingredients ingreds)
{
    switch(ingreds)
    {
        case FLOUR:
            return "Flour";
        case SUGAR:
            return "Sugar";
        case YEAST:
            return "Yeast";
        case BAKINGSODA:
            return "Baking Soda";
        case SALT:
            return "Salt";
        case CINNAMON:
            return "Cinnamon";
        case EGGS:
            return "Eggs";
        case MILK:
            return "Milk";
        case BUTTER:
            return "Butter";
        default:
            return "IDK THIS INGREDIENT";
    }
}


void getIngredientsFromFridge(int bakerID, recipe* rlist, recipeNames name)
{
    printf("Baker %d is waiting to open the fridge\n", bakerID);
    fridge.sem_op = WAIT;
    semop(fridgeID, &fridge, 1);
    printf("Baker %d has opened the fridge and is grabbing ingredients for %s from the fridge NOM NOM NOM.\n", bakerID, recipeNamesToString(name));
    for (int i = 0; i < rlist[name].numIngredients; i++){
        if (isInFridge(rlist[name].recipeIngredients[i])){
            printf("Baker %d grabbed the %s from the fridge for %s\n", bakerID, ingredientNamesToString(rlist[name].recipeIngredients[i]), recipeNamesToString(name));
            rlist[name].currentIngredients[i] = true;
        }
    }
    sleep(2);
    printf("Baker %d finished getting ingredients for %s from the fridge!\n", bakerID, recipeNamesToString(name));
    fridge.sem_op = SIGNAL;
    semop(fridgeID, &fridge, 1);
}

void getIngredientsFromPantry(int bakerID, recipe* rlist, recipeNames name)
{
    printf("Baker %d is waiting to enter the pantry\n", bakerID);
    pantry.sem_op = WAIT;
    semop(pantryID, &pantry, 1);
    printf("Baker %d has opened the pantry and is grabbing ingredients for %s from the pantry.\n", bakerID, recipeNamesToString(name));
    for (int i = 0; i < rlist[name].numIngredients; i++){
        if (isInPantry(rlist[name].recipeIngredients[i])){
            printf("Baker %d grabbed the %s from the pantry for %s\n", bakerID, ingredientNamesToString(rlist[name].recipeIngredients[i]), recipeNamesToString(name));
            rlist[name].currentIngredients[i] = true;
        }
    }
    sleep(3);
    printf("Baker %d finished getting ingredients for %s from the pantry!\n", bakerID, recipeNamesToString(name));
    pantry.sem_op = SIGNAL;
    semop(pantryID, &pantry, 1);
}

//this function will grab a mixer, then a bowl, then a spoon. The only limiting resource is the mixer so if a mixer is available, 
//then everything else should be avialable.
void mixIngredients(int bakerId, recipe* r, recipeNames name)
{
    //baker trys to grab the mixer
    printf("Baker %d is waiting for a mixer\n", bakerId);
    mixer.sem_op = WAIT;
    semop(mixerID, &mixer, 1);
    printf("Baker %d has grabbed a mixer to start mixing ingredients for %s\n", bakerId, recipeNamesToString(name));
    sleep(1);
    bowl.sem_op = WAIT;
    semop(bowlID, &bowl, 1);
    printf("Baker %d has grabbed a bowl for %s\n", bakerId, recipeNamesToString(name));
    sleep(1);
    spoon.sem_op = WAIT;
    semop(spoonID, &spoon, 1);
    printf("Baker %d has grabbed a spoon for  %s\n", bakerId, recipeNamesToString(name));
    sleep(1);
    printf("Baker %d is mixing the ingredients together for %s! MIX MIX MIX MIX MIX!\n", bakerId, recipeNamesToString(name));
    sleep(2);
    printf("Baker %d has finished mixing their ingredients for %s!\n", bakerId, recipeNamesToString(name));

    //free the semaphores, starting with the mixer
    mixer.sem_op = SIGNAL;
    printf("Baker %d has returned the mixer\n", bakerId);
    semop(mixerID, &mixer, 1);
    bowl.sem_op = SIGNAL;
    printf("Baker %d has returned the bowl\n", bakerId);
    semop(bowlID, &bowl, 1);
    spoon.sem_op = SIGNAL;
     printf("Baker %d has returned the spoon\n", bakerId);
    semop(spoonID, &spoon, 1);
    r->isMixed = true;
}

void bakeRecipe(int bakerId, recipe* r, recipeNames name)
{
    //baker is waiting for the oven
    printf("Baker %d is waiting for the sole, overworked oven\n", bakerId);
    oven.sem_op = WAIT;
    semop(ovenID, &oven, 1);
    printf("Baker %d has grabbed the sole, overworked, tired oven and started baking %s!\n", bakerId, recipeNamesToString(name));
    sleep(3);
    printf("Baker %d has finished cooking %s and the sole, dreary oven is ready for another beating\n", bakerId, recipeNamesToString(name));
    r->isBaked = true;
    oven.sem_op = SIGNAL;
    semop(ovenID, &oven, 1);
}

bool hasAllIngredients(recipe r)
{
    for(int i =0; i < r.numIngredients; i++)
    {
        if(r.currentIngredients[i] == true)
            continue;
        return false;
    }
    return true;
}

bool isBakerDone(recipe* bakersRecipeList)
{
    for(int i = 0; i < NUMRECIPES; i++)
    {
        if(bakersRecipeList[i].isBaked == true)
            continue;
        return false;
    }
    return true;
}

bool isInFridge(ingredients ingredient)
{
    return ingredient == EGGS || ingredient == BUTTER || ingredient == MILK;
}

bool isInPantry(ingredients ingredient)
{
    return !isInFridge(ingredient);
}

void initIngredients(recipe* rlist, recipeNames name){
     for (int i = 0; i < rlist[name].numIngredients; i++){
        rlist[name].currentIngredients[i] = false;
    }
    rlist[name].isMixed = false;
    rlist[name].isBaked = false;
}

void recipeInit(void){
    //cookies
    recipeList[COOKIES].numIngredients = 4;
    recipeList[COOKIES].recipeIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(ingredients));
    recipeList[COOKIES].currentIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(bool));
    recipeList[COOKIES].recipeIngredients[0] = FLOUR;
    recipeList[COOKIES].recipeIngredients[1] = SUGAR;
    recipeList[COOKIES].recipeIngredients[2] = MILK;
    recipeList[COOKIES].recipeIngredients[3] = BUTTER;
    initIngredients(recipeList, COOKIES);

    recipeList[PANCAKES].numIngredients = 7;
    recipeList[PANCAKES].recipeIngredients = malloc(recipeList[PANCAKES].numIngredients * sizeof(ingredients));
    recipeList[PANCAKES].currentIngredients = malloc(recipeList[PANCAKES].numIngredients * sizeof(bool));
    recipeList[PANCAKES].recipeIngredients[0] = FLOUR;
    recipeList[PANCAKES].recipeIngredients[1] = SUGAR;
    recipeList[PANCAKES].recipeIngredients[2] = MILK;
    recipeList[PANCAKES].recipeIngredients[3] = BUTTER;
    recipeList[PANCAKES].recipeIngredients[4] = BAKINGSODA;
    recipeList[PANCAKES].recipeIngredients[5] = EGGS;
    recipeList[PANCAKES].recipeIngredients[6] = SALT;
    initIngredients(recipeList, PANCAKES);

    recipeList[PIZZA].numIngredients = 3;
    recipeList[PIZZA].recipeIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(ingredients));
    recipeList[PIZZA].currentIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(bool));
    recipeList[PIZZA].recipeIngredients[0] = YEAST;
    recipeList[PIZZA].recipeIngredients[1] = SUGAR;
    recipeList[PIZZA].recipeIngredients[2] = SALT;
    initIngredients(recipeList, PIZZA);

    recipeList[PRETZELS].numIngredients = 6;
    recipeList[PRETZELS].recipeIngredients = malloc(recipeList[PRETZELS].numIngredients * sizeof(ingredients));
    recipeList[PRETZELS].currentIngredients = malloc(recipeList[PRETZELS].numIngredients * sizeof(bool));
    recipeList[PRETZELS].recipeIngredients[0] = FLOUR;
    recipeList[PRETZELS].recipeIngredients[1] = SUGAR;
    recipeList[PRETZELS].recipeIngredients[2] = SALT;
    recipeList[PRETZELS].recipeIngredients[3] = YEAST;
    recipeList[PRETZELS].recipeIngredients[4] = BAKINGSODA;
    recipeList[PRETZELS].recipeIngredients[5] = EGGS;
    initIngredients(recipeList, PRETZELS);

    recipeList[CINNAMONROLLS].numIngredients = 6;
    recipeList[CINNAMONROLLS].recipeIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(ingredients));
    recipeList[CINNAMONROLLS].currentIngredients = malloc(recipeList[COOKIES].numIngredients * sizeof(bool));
    recipeList[CINNAMONROLLS].recipeIngredients[0] = FLOUR;
    recipeList[CINNAMONROLLS].recipeIngredients[1] = SUGAR;
    recipeList[CINNAMONROLLS].recipeIngredients[2] = SALT;
    recipeList[CINNAMONROLLS].recipeIngredients[3] = BUTTER;
    recipeList[CINNAMONROLLS].recipeIngredients[4] = EGGS;
    recipeList[CINNAMONROLLS].recipeIngredients[5] = CINNAMON;
    initIngredients(recipeList, CINNAMONROLLS);
}

void initializeSemaphores(void)
{
    mixerID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (mixerID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(mixerID, 0, SETVAL, 2) < 0){
        perror("Failed to control resource");
        exit(1);
    }

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

    fridgeID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (fridgeID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(fridgeID, 0, SETVAL, 2) < 0){
        perror("Failed to control resource");
        exit(1);
    }

    bowlID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (bowlID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(bowlID, 0, SETVAL, 3) < 0){
        perror("Failed to control resource");
        exit(1);
    }

    spoonID = semget(IPC_PRIVATE, 1, S_IWUSR);
    if (spoonID < 0)
    {
        perror("Failed to create semaphore");
        exit(1);
    }
    if (semctl(spoonID, 0, SETVAL, 5) < 0){
        perror("Failed to control resource");
        exit(1);
    }

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
}
