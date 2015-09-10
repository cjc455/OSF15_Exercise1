#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

//this function creates a matrix, gives it a name, and sets its rows and cols
bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!new_matrix) {
		printf("ERROR: func create_matrix: input matrix is null");
		return false;
	}
	if(!name){
		printf("ERROR: func create_matrix: input name is null");
		return false;
	}
	if(rows == 0 || cols == 0) {
		printf("ERROR: func create_matrix: input cols or rows is 0");
		return false;
	}
	
	
	//calloc creates memory, calloc creates memory then sets values to 0
	//create memory for the sizeof 1 matric and initialize the values to 0
	*new_matrix = calloc(1,sizeof(Matrix_t));

	//check if the memory for new_matrix was created correctly
	if (!(*new_matrix)) {
		printf("ERROR: func create_matrix: could not correctly create new_matrix");
		return false;
	}

	//initilize and allocate memory for the data feild in new_matrix, enough to hold enough space for rows*cols
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));

	//Was that successful?
	if (!(*new_matrix)->data) {
		printf("ERROR: func create_matrix: could not initialize data for new_matrix");
		return false;
	}

	//set the rows and columns
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;

	//add extra null terminating character
	unsigned int len = strlen(name) + 1;

	//limit matrix name size and set its name
	if (len > MATRIX_NAME_LEN) {
		return false;
	}

	strncpy((*new_matrix)->name,name,len);
	return true;

}

	//TODO FUNCTION COMMENT


/*
*	PURPOSE: Clear the memory from a matrix
*	INPUTS: Pointer to a matrix
*	RETURN: void
*/
void destroy_matrix (Matrix_t** m) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!m){
		printf("ERROR: func destroy_matrix: input matrix is null");
		return;
	}
	
	//free is a function that clears the block of memory of the argument
	//It accepts any type of pointer
	//The function free knows how much memory to clear from a little bit of extra information
	//that was created when alloc or calloc was called.

	//First, free the data within the matrix
	free((*m)->data);

	//now clear the matrix itself
	free(*m);
	*m = NULL;
}


	
	//TODO FUNCTION COMMENT

/*
*	PURPOSE: This function takes two matrixies as arguments.
*	This function then checks if they are equal.
*	ARGUMENTS: Matrix a
*	MAtrix b to compare with matrix a
*	RETURN: Return false if the inputs are different, true if they are equal
*	for each value of each row and column
*
*
*/
bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a || !b || !a->data || !b->data) {
		printf("ERROR: func create_matrix: an input matrix or matrix data is null");
		return false;	
	}
	if(a->rows == 0 || a->cols == 0
	|| b->rows == 0 || b->cols == 0){
		printf("ERROR: func create_matrix: a row or column on input matrixies is 0");
		return false;
	}

	//results holds the comparison of the result of memcpy.
	//compares the values of a->data and b->data for the 3rd paramater number of bytes
	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		//return true if completley equal
		return true;
	}
	//matrixies are not equal
	return false;
}

	//TODO FUNCTION COMMENT

/*
*	PURPOSE: copy the contents of one matrix into anoth
*	INPUT: src, the source matrix, copying from
*	dest, the destination matrix copying to
*	RETURN: true if the src is correctly copied over to dest
*	false if there is an error	
*/
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	//TODO ERROR CHECK INCOMING PARAMETERS

	//Error: src or dest is null
	
	if (!src || !dest) {
		printf("ERROR: func duplicate_matrix: source or destination matrixies are invalid.");
		return false;
	}
	/*
	 * copy over data
	 */

	//find the ammount of spave one unsigned int takes.
	//then multiply that number by the ammount of unsigned ints a mtrix holds
	//copy the data from src to dest
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;

	//arguments: destination, source, number of bytes to copy
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	//TODO FUNCTION COMMENT
/*	PURPOSE:perform a bitwise shift on the bits in each value of the matrx left or right
*	INPUT: This function accepts an argument, direction, and int as an input.
*	Every value contained in the data structure will have their bits shifted
*	based on the the direction and shift values given.
*	Shift is a bitwise operator that moves all of the bits in a number left or right,
*	and adds 0's to the other end.
*	RETURN: true if the inputs are correct and the operation was sucessfuly, false otherwise
*/
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		printf("ERROR: func bitwise_shift_matrix: input matrix a is null");
		return false;
	}
	if(direction != 'l' && direction != 'L' && direction != 'r' && direction != 'R'){
		printf("ERROR: func bitwise_shift_matrx: input direction is not L l R or r");
		return false;
	}

	//first check direction
	if (direction == 'l' || direction == 'L') {
		//i is loop counter for rows
		unsigned int i = 0;
		//loop for each row in a
		for (; i < a->rows; ++i) {
			//j is loop counter for cols
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				//shift left and assign to its origional memory position
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	//repeat, but for right
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

	//TODO FUNCTION COMMENT
/*
*	PURPOSE: Adds two matrixies together and calculates the result
*	INPUT: Accepts three matrixies as arguments:
*	the first two are added together and the result is put into the third
*	RETURN: true if the matrixies can be added (have equal columns and rows) and the inputs are proper, false
*
*/
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!a){
		printf("ERROR: Funct add_matrices: input matrix a is null");
		return false;
	}
	if(!b){
		printf("ERROR: Funct add_matrices: input matrix b is null");
		return false;
	}
	if(!c){
		printf("ERROR: Funct add_matrices: input matrix c is null");
		return false;
	}
	if (a->rows != b->rows && a->cols != b->cols) {
		printf("ERROR: funct add_matricies: rows of a does not equal rows of b, or cols of a does not equal cols of b");
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	//TODO FUNCTION COMMENT
/*	PURPOSE: convert matrix m into a printable string, then print it
*	INPUT: Matrix to be displayed
*	RETURN: void
*/
void display_matrix (Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!m){
		printf("ERROR: func display_matrix: input matrix m is null");
		return;
	}

	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	//TODO FUNCTION COMMENT
/*	PURPOSE: reads a matrix from a file nammed matrix_input_filename
*	INPUT: c-string to filename, pointer to matrix to store loaded matrix
*	RETURN: true if load was successful and all inputs passed checks, false otherwise.
*/
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS


	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);

	if (close(fd)) {
		return false;

	}
	return true;
}

	//TODO FUNCTION COMMENT
/*	PURPOSE:copy a matrix variable into a file to be stored for later use
*	INPUTS: c-string containing the file name to be written
*	matrix m to store new data
*	RETURN: returns trues if file was correctly written and all inputs were correct.
	returns false if there was an error.
*/
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	//TODO FUNCTION COMMENT
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % end_range + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	//TODO FUNCTION COMMENT
void load_matrix (Matrix_t* m, unsigned int* data) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!m){
		printf("ERROR: function load_matrix: input matrix is null");
		return;
	}
	if(!data){
		printf("ERROR: function load_matrix: input data is null");
		return;
	}
	if(*data == 0) {
		printf("ERROR: function load_matrix: input data is equal to 0");
		return;
	}

	//this function copies the valies from data into m->data.
	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

	//TODO FUNCTION COMMENT
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	//this function adds the matrix new_matrix to the array mats	

	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!mats){
		printf("ERROR: function add_matrix_to_array: input matrix is null");
	}

	if(!new_matrix){
		printf("ERROR: function add_matrix_to_array: input new_matrix is null");
	}
	if(num_mats == 0){
		if(!mats){
		printf("ERROR: function add_matrix_to_array: input num_mats is 0");
	}
	}

	//index of matrix
	static long int current_position = 0;
	const long int pos = current_position % num_mats;

	//if matrix already exists, clear it out
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	}

	//set the locations in the array to the new matrix 
	mats[pos] = new_matrix;
	current_position++;

	//return index of new matrix
	return pos;
}
