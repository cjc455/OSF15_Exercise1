#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include<readline/readline.h>

#include "command.h"
#include "matrix.h"

void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats);
int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, 
			const char* target);

// TODO complete the defintion of this function. 
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats);

	//TODO FUNCTION COMMENT
	//PURPOSE: Main function is the main entry point of the program. It initializes the matrix array, and waits for user input to control which function to call next.
	//INPUT: the initial user arguments 
	//RETURN: returns an int that can be used as an error code when the function returns from an error.
int main (int argc, char **argv) {
	
	//srand sets up the use of the rand() function to generate random numbers
	srand(time(NULL));		
	char *line = NULL;
	Commands_t* cmd;

	//create an array of 10 matrixies
	Matrix_t *mats[10];

	//this call of memset sets all of the bytes for the first 10 instances of mats to have the value 0
	memset(&mats,0, sizeof(Matrix_t*) * 10); // IMPORTANT C FUNCTION TO LEARN

	Matrix_t *temp = NULL;

	//arguments for create_matrix
	//pointer to new matrix. This matrix will be created in the function
	//name of matrix
	//width, height
	bool created_matrix = create_matrix (&temp,"temp_mat", 5, 5); // TODO ERROR CHECK
	if(!created_matrix) {
		printf("ERROR: func main: failed to create temp matrix.");
		return -1;
	}
	//call this function in file matrix.c
	add_matrix_to_array(mats,temp, 10); //TODO ERROR CHECK NEEDED
	int mat_idx = find_matrix_given_name(mats,10,"temp_mat");

	if (mat_idx < 0) {
		perror("PROGRAM FAILED TO INIT\n");
		return -1;
	}
	random_matrix(mats[mat_idx], 10, 15);
	if(!write_matrix("temp_mat", mats[mat_idx])) {// TODO ERROR CHECK
		printf("ERROR: func main: could not write matrix.");
		return -1;
	}

	line = readline("> ");
	while (strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("Failed at parsing command\n\n");
		}
		
		if (cmd->num_cmds > 1) {	
			run_commands(cmd,mats,10);
		}
		if (line) {
			free(line);
		}
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	destroy_remaining_heap_allocations(mats,10);
	return 0;	
}

	//TODO FUNCTION COMMENT
	//PURPOSE: Handles a list of input commands, and takes the appropriate action to execute the desired user command.
	//INPUT: a Commands_t type, which contains the commands, and the arguements of the command typed by the user.
	//mats, which is a list of matrixies, so the matrixies can be modified by the commands
	//num_mats, the number of matrixies within the double pointer mats
	//return: void, but mats is modified usually
void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats) {
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!cmd){
		printf("ERROR: func run_commands: input Commands_t is null");
		return;
	}
	if(!mats){
		printf("ERROR: funct run_commands: mats is null");
		return;
	}
	if(num_mats == 0){
		printf("ERROR: func run_commands: num_mats is 0.");
		return;
	}

	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/
			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (mats[idx]);
			}
			else {
				printf("Matrix (%s) doesn't exist\n", cmd->cmds[1]);
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					printf("Failure to create the result Matrix (%s)\n", cmd->cmds[3]);
					return;
				}
			
				int add_location = add_matrix_to_array(mats,c, num_mats); //TODO ERROR CHECK NEEDED


				if(add_location < 0) {
					printf("ERROR: func run_commands: failur in function add_matrix_to_array.");
					return;
				}
				if (! add_matrices(mats[mat1_idx], mats[mat2_idx],c) ) {
					printf("Failure to add %s with %s into %s\n", mats[mat1_idx]->name, mats[mat2_idx]->name,c->name);
					return;	
				}
			}
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 3 && strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if (mat1_idx >= 0 ) {
				Matrix_t* dup_mat = NULL;
				if( !create_matrix (&dup_mat,cmd->cmds[2], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					return;
				}
				if(!duplicate_matrix (mats[mat1_idx], dup_mat)) //TODO ERROR CHECK NEEDED
				{
					printf("ERROR: func run_commands: could not duplicate_matrix.");
					return;
				}
				if(0 > add_matrix_to_array(mats,dup_mat,num_mats)) { //TODO ERROR CHECK NEEDED
					printf("ERROR: func run_commands: could not add matrix to array");
					return;
				}
				printf ("Duplication of %s into %s finished\n", mats[mat1_idx]->name, cmd->cmds[2]);
		}
		else {
			printf("Duplication Failed\n");
			return;
		}
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 2) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				if ( equal_matrices(mats[mat1_idx],mats[mat2_idx]) ) {
					printf("SAME DATA IN BOTH\n");
				}
				else {
					printf("DIFFERENT DATA IN BOTH\n");
				}
			}
			else {
				printf("Equal Failed\n");
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const int shift_value = atoi(cmd->cmds[3]);
		if (mat1_idx >= 0 ) {
			if(!bitwise_shift_matrix(mats[mat1_idx],cmd->cmds[2][0], shift_value)){ //TODO ERROR CHECK NEEDED
				printf("ERROR: function handle_input: failed to shift bits.");
				return;
			}
			printf("Matrix (%s) has been shifted by %d\n", mats[mat1_idx]->name, shift_value);
		}
		else {
			printf("Matrix shift failed\n");
			return;
		}

	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			printf("Read Failed\n");
			return;
		}	
		
		if(0 > add_matrix_to_array(mats,new_matrix, num_mats)){ //TODO ERROR CHECK NEEDED
			printf("failed to add matrix to array.");
			return;
		}
		printf("Matrix (%s) is read from the filesystem\n", cmd->cmds[1]);	
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx]->name,mats[mat1_idx])) {
			printf("Write Failed\n");
			return;
		}
		else {
			printf("Matrix (%s) is wrote out to the filesystem\n", mats[mat1_idx]->name);
		}
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN && cmd->num_cmds == 4) {
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);

		if(!create_matrix(&new_mat,cmd->cmds[1],rows, cols)) //TODO ERROR CHECK NEEDED
		{
			printf("ERROR: could not create new matrix.");
			return;
		}
		if(0>add_matrix_to_array(mats,new_mat,num_mats)){ //TODO ERROR CHECK NEEDED
			printf("failed to add matrix to array.");
			return;
		}
		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	else if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		if(!random_matrix(mats[mat1_idx],start_range, end_range)){ //TODO ERROR CHECK NEEDED
			printf("Error creating random matrix.");
			return;
		}

		printf("Matrix (%s) is randomized between %u %u\n", mats[mat1_idx]->name, start_range, end_range);
	}
	else {
		printf("Not a command in this application\n");
	}

}

	//TODO FUNCTION COMMENT
	/*PURPOSE: finds the location of the matrix with the name equal to the input c-string
	*INPUT: matrix list to look for correct matrix,
	*the length of the input matrix
	*the name of the matrix searching for
	*RETURN: an  int equal to the position of the matrix if it was found correctly.
	*return -1 if the matrix was not found
	*return -2 if there was an input error
	*/

int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, const char* target) {
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!mats){
		return -2;
	}
	if(num_mats == 0){
		return -2;
	}
	if(!target){
		return -2;
	}
	if(strlen(target) == 0){
		return -2;
	}
	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i]->name,target,strlen(mats[i]->name)) == 0) {
			return i;
		}
	}
	return -1;
}

	//TODO FUNCTION COMMENT
	//PURPOSE: free all of the memory used by all of the matrixies
	//INPUT: the matrix array, and the number of matrixies
	//RETURN: void. The matrix is now empty.
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	// COMPLETE MISSING MEMORY CLEARING HERE
	if(!mats){
		printf("ERROR: func destroy_remaining_heap_allocations: input matrix array is null");
		return;
	}
	if(num_mats == 0) {
		printf("WARNING: func destroy_remaining_heap_allocations: num_mats is 0");
		return;
	}
	int c = 0;
	for(c = 0; c < num_mats; c++) {
		free((*mats)->data);
		free((*mats)->name);
		free(*mats);
	}
	free(mats);
}
