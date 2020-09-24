#include<pthread.h>
//Standard io/library
#include<stdio.h>
#include<stdlib.h>

//Defining global variable of total thread count for check sudoku
//Defining global variable of puzzle size 
#define NUMBER_OF_THREADS 19
#define PUZZLE_SIZE 9

//Defining our puzzle with size 9+1 x 9+1 
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
            {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
            {-1,5,3,4,6,7,8,9,1,2},
            {-1,6,7,2,1,9,5,3,4,8},
            {-1,1,9,8,3,4,2,5,6,7},
            {-1,8,5,9,7,6,1,4,2,3},
            {-1,4,2,6,8,5,3,7,9,1},
            {-1,7,1,3,9,2,4,8,5,6},
            {-1,9,6,1,5,3,7,2,8,4},
            {-1,2,8,7,4,1,9,6,3,5},
            {-1,3,4,5,2,8,6,1,7,9}
};

//Array to hold answers to each threads
int final_result[NUMBER_OF_THREADS] = {0};

//Function to see if row is valid 
void* row_valid(void* param);

//Function to see if column is valid
void* col_valid(void* param);

//Function to see if the region is valid
void* region_valid(void* param);

//Structure that has the parameter of row and column with thread number
typedef struct{
    int row;
    int col;
    int thread_number;
} parameters;

int main(){

    //Initialize thread array with space of 11
    pthread_t threads[NUMBER_OF_THREADS];

    //Checking all COLs
    //Creating a variable of type parameter 
    parameters* data = (parameters *) malloc(sizeof(parameters));
    int thread_number1 = 0;
    for(int i = 1; i < PUZZLE_SIZE+1; ++i){
        //Giving that variable data
        data = (parameters *) malloc(sizeof(parameters));
        data->row = 1;
        data->col = i;
        data->thread_number = thread_number1;
        //Creating the thread with the specific information
        //            (thread, NULL, Function, Parameter)
        pthread_create(&threads[thread_number1], 0, col_valid, data);
        thread_number1++;
    }

    //Checking all ROWs
    int thread_number2 = 9;
    for(int i = 1; i < PUZZLE_SIZE+1; ++i){
        data = (parameters *) malloc(sizeof(parameters));
        data->row = i;
        data->col = 1;
        data->thread_number = thread_number2;
        pthread_create(&threads[thread_number2], 0, row_valid, data);
        thread_number2++;
    }

    //Checking all REGIONs
    int thread_number3 = 18;
    //(i,j) -> Regions
    //(1,1) -> (1,4) -> (1,7) 
    //(4,1) -> (4,4) -> (4,7)
    //(7,1) -> (7,4) -> (7,7)
    for(int i = 1; i < PUZZLE_SIZE + 1; i+=3){
        for(int j = 1; j < PUZZLE_SIZE + 1; j+=3){
                data = (parameters *) malloc(sizeof(parameters));
                data->row = i;
                data->col = j;
                data->thread_number = thread_number3;
                pthread_create(&threads[thread_number3], 0, region_valid, data);
                thread_number3++;
        }
    }

    //Wait for all threads to finish
    for(int i = 0; i < NUMBER_OF_THREADS; ++i){
        pthread_join(threads[i], 0);
    }

    //Verdict variable to check if all threads are valid
    int verdict = 1;
    for(int i = 0; i < NUMBER_OF_THREADS; ++i){
        if(final_result[i] == 0){
            verdict = 0;
            break;
        }
    }

    //Using verdict to dictate if Sudoku is valid or not
    if(verdict){
        printf("Sudoku is valid! \n");
    } else {
        printf("Sudoku is Not valid! \n");
    }

    return 0;
}

//Checking all columns
void* col_valid(void* param){
    parameters* position = (parameters*)param;

    int col_result[PUZZLE_SIZE+1] = {0};
    for(int i = 1; i <PUZZLE_SIZE+1; ++i){
        col_result[puzzle[i][position->col]] = 1;
    }

    int col_good = 1;
    for(int j = 1; j < PUZZLE_SIZE+1; ++j){
        if(col_result[j] == 0){
            col_good = 0;
            break;
        }
    }

    if(col_good){
        final_result[position->thread_number] = 1;
        printf("col %d is good, %d \n", position->col, position->thread_number);
    } else {
        printf("%d col is bad, %d \n", position->col, position->thread_number);
    }

    pthread_exit(0);
}

//Checking all rows
void* row_valid(void* param){
 parameters* position = (parameters*)param;

    int row_result[PUZZLE_SIZE+1] = {0};
    for(int i = 1; i <PUZZLE_SIZE+1; ++i){
        row_result[puzzle[position->row][i]] = 1;
    }

    int row_good = 1;
    for(int j = 1; j < PUZZLE_SIZE+1; ++j){
        if(row_result[j] == 0){
            row_good = 0;
            break;
        }
    }

    if(row_good){
        final_result[position->thread_number] = 1;
        printf("row %d is good, %d \n", position->row, position->thread_number);
    } else {
        printf("%d row is bad, %d \n", position->row, position->thread_number);
    }
 
    pthread_exit(0);
}

void* region_valid(void* param){
    //Setting position to param's data
    //data->row = 1
    //data->col = 4
    //data->thread_number = 4 (Second region's information)
    parameters* position = (parameters*)param;

    //each_region array variable to hold validation of all numbers inside the region
    int each_region[PUZZLE_SIZE+1] = {0};
    //For loop to check the first number from the top right position going right to left
    //Assigning each number present a 1 if its in the region
    for(int i = position->row; i < position->row + 3; ++i){
        for(int j = position->col; j < position->col + 3; ++j){
            each_region[puzzle[i][j]] = 1;
        }
    }

    //Variable to check if the ONE region is good
    int one_region_good = 1;
    //Loop to check if all the numbers in the region is present, if not, assign this region a 0
    for(int j = 1; j < PUZZLE_SIZE + 1; ++j){
        if(each_region[j] == 0){
            one_region_good = 0;
            break;
        }
    }

    //If this region is good, assigning the thread number for this thread a 1.
    if(one_region_good){
        final_result[position->thread_number] = 1;
    } else {
        printf("Region (%d, %d) is bad. \n", position->row, position->col);
    }

    pthread_exit(0);
}
