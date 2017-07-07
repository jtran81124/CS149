#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define MAXLINE 80 /* The maximum length command */

void executeArgs(char* args[], int flag);
int getArgs(char* args[]);

int main(void){
	
	char* args[MAXLINE / 2 + 1];
	int should_run = 1;
	int argCount;

	printf("CS149 Shell from Michael Nguyen\n");
	while(should_run){
		printf("Michael-819>");
		fflush(stdout);
		argCount = getArgs(args);	//return an int so we know where & is
									//& will always be the last index
		if(strcmp(args[0], "exit") == 0)	//check for exit command
			should_run = 0;
		else if(strcmp(args[argCount - 1], "&") == 0){	//& will set off the flag
			args[argCount - 1] = NULL;
			executeArgs(args, 1);
		}
		else
			executeArgs(args, 0);
	}
	return 0;
}
void executeArgs(char* args[], int flag){
	pid_t pid;
	int status;
		
	pid = fork();
	if(pid == 0 && !flag){		//child process was created
		if(execvp(args[0],args) < 0){	//if execvp() fails
			printf("ERROR: execvp failed! %s!\n", strerror(errno));
			exit(1); //exec fail will exit out and go to next prompt
		}
	}
	//if the command contains &, do not wait for parent
	else if(pid == 0 && flag){
		if(execvp(args[0], args) < 0){
			printf("ERROR: execvp failed! %s!\n", strerror(errno));
			exit(1);
		}
		else
			exit(0);
	}
	//child process was not created
	else if(pid < 0)
		printf("ERROR: forking child process failed! %s!\n", strerror(errno));
	else if(!flag)				//parent waits for child to complete
		wait(&status);
}
int getArgs(char* args[]){
	int i = 0;
	char str[MAXLINE / 2 + 1];
	char* token;

	if(fgets(str, sizeof(str), stdin) != NULL){
		//tokenize the first argument
		token = strtok(str, " \n");
		//while there are tokens, store into args[]
		while(token != NULL){
			if(token != NULL){
				args[i] = malloc(strlen(token) + 1);	//allocate +1 for NULL
				strcpy(args[i], token);
			}
			token = strtok(NULL, " \n");
			i++;
		}
		//null terminate so execvp knows where the array stops
		args[i] = NULL;
	}
	return i;
}
