#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25


	//TODO FUNCTION COMMENT
/*
*	PURPOSE: parse the user input into words that can be easier analyzed by the code, rather than a single long string
*	INPUT: A const c-string for the user input
	A double pointer to a Commands_t that 
*/

bool parse_user_input (const char* input, Commands_t** cmd) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	
	if(!input){
		printf("ERROR: function parse_user_input: input is null");
		return false;
	}
	if(!cmd){
		printf("ERROR: func parse_user_input: cmd is null");
		return false;
	}
	char *string = strdup(input);
	if(!string) {
		printf("ERROR: func parse_user_input: string is null");
		return false;
	}
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

	//TODO FUNCTION COMMENT

	//PURPOSE: Free the memory that has already been allocated by an instance of Commands_t
	//INPUT: the instance of Commands_t to be destroyed
	//RETURN: void
void destroy_commands(Commands_t** cmd) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!cmd){
		printf("ERROR: funct destroy_commands: cmd is null");
		return;
	}	

	for (int i = 0; i < (*cmd)->num_cmds; ++i) {
		free((*cmd)->cmds[i]);
	}
	free((*cmd)->cmds);
	free((*cmd));
	*cmd = NULL;
}

