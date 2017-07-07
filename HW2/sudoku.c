#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * This is the structure that passes data to the thread
 */
typedef struct{
	int row;
	int col;
} parameters;

void printSudoku();
parameters** init();
void* checkCol(void* param);
void* checkRow(void* param);
void* checkSubGrid(void* param);

const int sudoku[9][9] = {{6,5,3,1,2,8,7,9,4}, 
						  {1,7,4,3,5,9,6,8,2}, 
						  {9,2,8,4,6,7,5,3,1},
						  {2,8,6,5,1,4,3,7,9},
						  {3,9,1,7,8,2,4,5,6},
						  {5,4,7,6,9,3,2,1,8},
						  {8,6,5,2,3,1,9,4,7},
						  {4,1,2,9,7,5,8,6,3},
						  {7,3,9,8,4,6,1,2,5}};
#define NUM_THREADS 11
#define NUM_SUBGRIDS 9

int main(void){
	pthread_t worker[NUM_THREADS]; //array of worker threads
	parameters** gridArr; //array of parameters* for 9 subgrids
	int i;
	//array to store result from worker threads	
	int count = 0;
	void* returnVal;

	gridArr = init(); //initiate array of parameters to use
	printf("CS149 Sudoku from Michael Nguyen\n");
	printSudoku();

	//create threads for row and col check
	parameters* data = (parameters*) malloc(sizeof(parameters));
	data->row = 0;
	data->col = 0;

	pthread_create(&worker[0], NULL, checkRow, (void*) data);
	pthread_create(&worker[1], NULL, checkCol, (void*) data);
	//create threads for subgrids
	for(i = 2; i < NUM_THREADS; i++)
		pthread_create(&worker[i], NULL, checkSubGrid, (void*) gridArr[i - 2]);

	//wait until the worker threads finish processing
	//passes back result of thread into sol array
	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(worker[i], &returnVal);
		if((int)returnVal == 1)
			count++; //if count != 11, then the solution is invalid
	}

	// free all data
	free(data);
	for(i = 0; i < NUM_SUBGRIDS; i++)
		free(gridArr[i]);
	free(gridArr);

	if(count == 11)
		printf("This is a valid solution!\n");
	else
		printf("This is not a valid solution!\n");

	return 0;
}
/*
 * Function to initialize the subgrids starting points
 * Returns an array of subgrids
 */
parameters** init(){
	//0 and 1 are the row and col threads respectively
	//the rest of the worker threads are for subgrids
	parameters** gridArr = (parameters**) malloc(sizeof(parameters) * NUM_SUBGRIDS);

	parameters* grid1 = (parameters*) malloc(sizeof(parameters));
	parameters* grid2 = (parameters*) malloc(sizeof(parameters));
	parameters* grid3 = (parameters*) malloc(sizeof(parameters));
	parameters* grid4 = (parameters*) malloc(sizeof(parameters));
	parameters* grid5 = (parameters*) malloc(sizeof(parameters));
	parameters* grid6 = (parameters*) malloc(sizeof(parameters));
	parameters* grid7 = (parameters*) malloc(sizeof(parameters));
	parameters* grid8 = (parameters*) malloc(sizeof(parameters));
	parameters* grid9 = (parameters*) malloc(sizeof(parameters));

	grid1->row = 0;
	grid1->col = 0;
	gridArr[0] = grid1;
	
	grid2->row = 0;
	grid2->col = 3;
	gridArr[1] = grid2;

	grid3->row = 0;
	grid3->col = 6;
	gridArr[2] = grid3;
	
	grid4->row = 3;
	grid4->col = 0;
	gridArr[3] = grid4;

	grid5->row = 3;
	grid5->col = 3;
	gridArr[4] = grid5;

	grid6->row = 3;
	grid6->col = 6;
	gridArr[5] = grid6;

	grid7->row = 6;
	grid7->col = 0;
	gridArr[6] = grid7;

	grid8->row = 6;
	grid8->col = 3;
	gridArr[7] = grid8;

	grid9->row = 6;
	grid9->col = 6;
	gridArr[8] = grid9;

	return gridArr;
}
void* checkCol(void* param){
	int i, j;
	parameters *data = (parameters*) param;
	//set the starting point for row and column
	int scol = data->col;
	int srow = data->row;
	
	for(i = scol; i < 9; i++){
		//array size of 10 so we can get index 9
		//will check to make sure no duplicate values exist in column
		int col[10] = {0,0,0,0,0,0,0,0,0,0};
		for(j = srow; j < 9; j++){
			int val = sudoku[j][i];
			//if it's 1 in the array there exists a duplicate value
			//return (void*) 0 to exit out
			if(col[val] != 0)
				return (void*) 0;
			else
				col[val] = 1;
		}
	}
	return (void*) 1;
}
void* checkRow(void* param){
	int i, j;
	parameters *data = (parameters*) param;
	//set the starting point for row and column
	int scol = data->col;
	int srow = data->row;

	for(i = srow; i < 9; i++){
		//array size of 10 so we can get index 9
		//will check to make sure no duplicate values exist in row
		int row[10] = {0,0,0,0,0,0,0,0,0,0};
		for(j = scol; j < 9; j++){
			int val = sudoku[i][j];
			//if it's 1 in the array there exists a duplicate value
			//return (void*) 0 to exit out
			if(row[val] != 0)
				return (void*) 0;
			else
				row[val] = 1;
		}
	}
	return (void*) 1;
}
void* checkSubGrid(void* param){
	int i, j;
	parameters *data = (parameters*) param;
	//set the starting point for row and column
	int scol = data->col;
	int srow = data->row;
	//array size of 10 so we can get index 9
	//will check to make sure no duplicate values exist in the subgrid
	int grid[10] = {0,0,0,0,0,0,0,0,0,0}; //not resetting per col or row so we init outside loop

	for(i = srow; i < srow + 3; i++){
		for(j = scol; j < scol + 3; j++){
			int val = sudoku[i][j];
			//if it's 1 in the array there exists a duplicate value
			//return (void*) 0 to exit out
			if(grid[val] != 0)
				return (void*) 0;
			else
				grid[val] = 1;
		}
	}
	return (void*) 1;
}
void printSudoku(void){
	int i, j;
	printf("----------------------");
	for(i = 0; i < 9; i++){
		if(i % 3 == 0 && i != 0)
			printf("----------------------");
		printf("\n");
		for(j = 0; j < 9; j++){
			if(j % 3 == 0 && j != 0)
				printf("| ");
			printf("%d ", sudoku[i][j]);
		}
		printf("\n");	
	}
	printf("----------------------\n");
}

