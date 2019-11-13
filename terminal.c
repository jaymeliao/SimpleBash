#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CHARS 256
#define MAX_ARGS 32

// ==============================================
char* getLine(char*);
//int copyNextArgument(char*, char**);
void printArg(char*);
// =============================================




int readCommand(char*[]);
void typePrompt();
void changeDir(char*[]);
void createChildProcess(char*[]);
void freeArgs(char*[], int);

int main()
{
	printf("\n");
	printf("--- Entering JAYME shell program (v 1.0.0) ---\n");
	printf("\n");

	char *args[MAX_ARGS] = {};

	while (1)
	{
		typePrompt();
		
		int nArgs = readCommand(args);
		if (nArgs == 0) continue;
		//printf("The number of arguments read is %d\n", nArgs);
		//args[nArgs] = NULL;
		
		if (strncmp(args[0],"cd",2)==0)
		{
		  //printf("Trying to change the directory with command %s\n", args[0]);
			changeDir(args);
		} else if (strncmp(args[0],"exit",4)==0) {
		  //printf("Trying to exit the shell with command %s\n", args[0]);
		  break;
		} else {
			createChildProcess(args);
		}
		// do not allow memory leaks when we iterate
		freeArgs(args, nArgs);
	}

	printf("\n");
	return 0;
}

void typePrompt()
{
	char currentDir[2048];
	getcwd(currentDir, sizeof(currentDir));
	printf("%s |>>>: ", currentDir);
}

void changeDir(char *args[])
{
	if (args[1] != NULL && strncmp(args[1],"\0",1)!=0)
		chdir(args[1]);
	else
		printf("Error using cd: need a path argument specified.\n");
}

// TO DO: complete createChildProcess function below; remove comments when done
void createChildProcess(char *args[])
{
	int status = 0;
	int pid; // we can keep track of a specific child process with this
	if ((pid = fork()) != 0)
	{
		/* Parent Code */
		int w = waitpid(pid, &status, 0);
		// two cases to deal with: 
		// waitpid returns -1 (not succussful), 
		if (w == -1) {
	                perror("waitpid");
        	        exit(EXIT_FAILURE);
            	}
		// or not (check value of WEXITSTATUS(status), which should be 101)
		if (WEXITSTATUS(status) == 101) {
			printf("Command not found!\n");
		}
		
		
		
	} else {
		/* Child Code */
		//printf("Trying to execute command %s\n", args[0]);
		
		// setup your env array of strings here
		char *env[] = { NULL };
		
		// make your call to execvpe
		execve(args[0], args, env);


		// --- child code will be replaced by exec, so you know there is an error if execution gets past here ---
		//printf("Exec call error and child did not replace parent's copy of code\n");
		exit(101); // error code sent to parent
		// the 101 code above is just for example
		// it is a nice trick to use powers of 2 to bitwise collect multiple errors in parent for many children, or threading, etc.
	}
}




// functions from getInputLine.c
// ===========================================================================================================




// TO DO: complete the readCommand function; remove comments when done
int readCommand(char *args[])
{
	char* input = getLine(input);
	
	if (input == NULL) 
	{
		//printf("Empty!\n");
		args[0] = NULL;
		return 0;
	}
	if ((input[0] == '\0') || (input[0] == '\n')) {
		printf("Empty line!\n");
		args[0] = NULL;
		return 0;
	}
	
	//printArg(input);	

	//char* dest = (char*)malloc(MAX_CHARS*sizeof(char));
	//int count = copyNextArgument(input, dest);
	//printArg(dest);
	//return 0;

	
	// use your copyNextArgument function to help you parse user input line
	char* source = input;
	int nArgs = 0;
	while (1) {
		// trim the beginning
		int count = trimBeginning(source);
		if (count == -1) break;
		source = source + count;
		
		// extract the args
		char* dest = (char*)malloc(MAX_CHARS*sizeof(char));
		count = copyNextArgument(source, dest);
		if (count == 0) break;
		source = source + count;

		args[nArgs] = dest;
		nArgs++;
	}
	 

	// debugging
	/*for(int i=0;i<nArgs;i++)
		printArg(args[i]);*/

	// ==================================
	
	free(input);
	// make sure to return the number of arguments parsed (including the first argument: the command)
	return nArgs;
}

char* getLine(char *input)
{
	input = (char*)malloc(MAX_CHARS*sizeof(char));
	fgets(input, MAX_CHARS, stdin);
	//printf("You typed command: %s\n", input);
	return input;
}

// TO DO: complete the copyNextArgument function
// a function for parsing each argument in the user input line
int copyNextArgument(char *source, char *dest) {
	int count = 0;
	while
	(
		source[count] != ' ' && // do not want spaces in an arg
		source[count] != '\0' && // do not want null character
		source[count] != '\n' // do not want end of line
	) count++;

	//printf("copyN says: count is %d\n",count);
	
	// use the strncpy function to help you; beware that it does not append a null character at end of strings (which you need)
	strncpy(dest, source, count);
	dest[count] = '\0';
	
	//printArg(dest);
  
	return count;
}

// getting rid of initial empty characters
// return: 
//	-1 => end of the string reached
//	n => number of empty initial characters

int trimBeginning(char *source) {
	int count = 0;
	if (source[count] == '\0') return -1;
	
	while
	(
		source[count] == ' ' || // do not want spaces in an arg
		source[count] == '\n' // do not want end of line
	) {
		if (source[count] == '\0') return -1;
		count++;
	}
	if (source[count] == '\0') return -1;
	return count;
}

// each element in args is a pointer that is managing memory for C-strings we need to free
void freeArgs(char *args[], int nArgs)
{
	int i;
	for (i = 0; i < nArgs; i++)
	{
		free(args[i]);
		args[i] = NULL;
	}
	// we do not need to free the array itself, since we are reusing it
}

// to double check each argument is being parsed okay, character by character
void printArg(char *arg)
{
	int i = 0;
	printf("Parsed argument: ");
	while (arg[i] != '\0')
	{
		printf("%c", arg[i++]);
	}
	printf("--\n");
}

