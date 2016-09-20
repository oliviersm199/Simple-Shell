#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "list.h"

/*
Olivier Simard-Morissette
260563480
ECSE 427
February 12th, 2016
*/

#define COMMAND_RANGE 10
#define MAX_ARGS 20
#define START_NUMBER 1

int getcmd(char *prompt, char *args[], int *background);
int loadCmd(char * prompt, char *args[], int *background);
int execBuildInCmd(char * args[]);
int freecmd(char * args[]);
int exitCmd();
int isNumber(char *arg);

int main(){
    char *args[MAX_ARGS];
    int cmdCount = START_NUMBER;
    while(1){
        int bg;
        int cnt = getcmd("\n>>  ", args, &bg);        
	//checking if value at index zero is a number for our history function
	if(isNumber(args[0])){
            int value = atoi(args[0]);
	    if(value<1||value>cmdCount){
	        printf("Erroneous value for history command");
		continue;
	    }	
	    else if(value<=cmdCount && value > (cmdCount-COMMAND_RANGE)){
	        //activate the history feature where we get the command
		Node * temp = getNodeData(value);
		int i;
		//implementation found in list.c
    		transferStrings(args,(temp->data),20);
		bg = temp->bg;	
            }
	    //user referenced an old command that is no longer considered.
	    //also considered erroneous and doesn't get saved so we continue
	    else{
		printf("Only %d most recent amounts are considered",COMMAND_RANGE);
		continue;
	    }
	}
	//store the command in args using push from list.c
	push(args,bg,20);
	cmdCount++;
	
	//try to execute the built in command
	int built_in = execBuiltInCmd(args);

	//this means that built in was not able to find a suitable command that
	//matched the parse string. 
	if(built_in == 1){
	    int status;
	    //if the function is not built in we fork
	    //and call execvp() 
	    int pid = fork();
	   
	    if (pid == 0) {
                //Child Process
		int saved_stdout = NULL;
	  	int argFileNameNum = -1;


		//checking to see if output redirection is asked in args.
		for(int i =0;args[i]!=NULL;i++){
		    if(strcmp(args[i],">")==0){
			argFileNameNum = i;
		    }
		}
		

		//going to close stdout and redirect output.
		if(argFileNameNum>=0){
		    if(args[argFileNameNum+1]==NULL){
			perror("Must give filename to output to.");
			continue;
		    }
		    saved_stdout = dup(1);
		    int filedesc = open(args[argFileNameNum+1],O_CREAT |O_WRONLY,S_IRUSR|S_IWUSR);
		    dup2(filedesc, 1);
		    //need to change args so that args with does not contain >
		    args[argFileNameNum] = NULL;
		}
		
		//executing command.
		if(execvp(args[0],args) == -1){
		    perror(" Error in command execution ");
		}
		//opening stdout again.
		if(saved_stdout!=NULL){
			dup2(saved_stdout, 1);
			close(saved_stdout);
		}		
	    }	
            //checking if there was an error in the forking. 
    	    else if (pid < 0){
	        perror("Error in forking.");
    	    }
    	    else{
	        //Parent Process
       	  	if(bg==0){
                    do{
		        waitpid(pid,&status,WUNTRACED);
        	    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
    	    }
	}
	//Was not sure how to implement and unfortunately did not complete. 
	//freecmd(args);
    }
    return 1;
}


//checks if a str is a number 
int isNumber(char * str){
	for(int i = 0 ;str[i]!='\0';i++){
		if(isdigit(str[i]) == 0){
			return 0;
		}	
	}
	return 1;
}

int freecmd(char * args[]){
	free(args);
	return 0;
}

//this command dictates which built in commands are supported
int execBuiltInCmd(char * args[]){
    char *command = args[0];
    
    if(strcmp(command,"pwd")==0){
	if(currentDirectory()<0){
	    return -1;
	}
	return 0;
    }	

    else if(strcmp(command,"cd")==0){
	if(changeDr(args[1])<0){
	   return -1;
	}
        return 0;	
    }

    else if(strcmp(command,"exit")==0){
        exitCmd();
	return 0;
    }

    else if(strcmp(command,"jobs")==0){
        jobs();
	return 0;
    }

    else if(strcmp(command,"fg")==0){
	if(fg(args[1])<0){
	    return -1;
	}
	return 0;
    }
    else if(strcmp(command,"history")==0){
	history();
	return 0;
    }
    return 1;
}
//the following are my built in functions

int history(){
	
	Node *temp = getNodeData(1);
	int counter = 1;
	
	//constant string to help with creating space between commands
	char * spaceConst=" ";

	printf("History\n");	
	while( (temp->next) != NULL){
		    //converting my array of strings into a single string
		    //for easy printing. 
		    char **tempData = (temp -> data);
		    char *tempStr;
		    
		    int strLength = 0;
		    int i = 0;

		    while(tempData[i]!=NULL){
			//add an additional one for the spaces in your commands
			//and the terminating character for the final string.
			strLength += strlen(tempData[i]+1);
			i++;
		    }

		    //allocate sufficient space for the initial string
		    tempStr = malloc((sizeof(char)*strLength)+1);

	            //initialize a counter that will go through the string.
		    int ctr = 1;
		    
		    //copy the first string
		    strcpy(tempStr,tempData[0]);
		    
	            //concatenate subsequent strings.
		    while(tempData[ctr]!=NULL){
			strcat(tempStr,spaceConst);
			strcat(tempStr,tempData[ctr]);
			ctr++;
		    }
		   printf("%s\n",tempStr,counter);
		   free(tempStr);
		temp = temp -> next;
		counter++;
	}
	printf("\n");
	return 1;
}
int changeDr(char *arg){
	if(arg == NULL){
		perror("CD: Expected a pathname");
	}
	if(chdir(arg) != 0){
		return -1;
	}
	return 1;
}

int currentDirectory(){
	char cwd[2048];
	if(getcwd(cwd,sizeof(cwd)) != NULL){
		fprintf(stdout,"%s",cwd);
	}
	else{
		perror("getcwd() error");
		return -1;
	}
	return 1;
}

int exitCmd(){
	exit(0);
}

int jobs(){
	//the point of this method is to return the values that are 	
	//get the first node 
	Node *temp = getNodeData(1);
	int counter = 1;
	
	//constant string to help with creating space between commands
	char * spaceConst=" ";

	printf("Jobs\t\tJob Number\n");	
	while( (temp->next) != NULL){
		if(temp->bg == 1){
		    //converting my array of strings into a single string
		    //for easy printing. 
		    char **tempData = (temp -> data);
		    char *tempStr;
		    
		    int strLength = 0;
		    int i = 0;

		    while(tempData[i]!=NULL){
			//add an additional one for the spaces in your commands
			//and the terminating character for the final string.
			strLength += strlen(tempData[i]+1);
			i++;
		    }

		    //allocate sufficient space for the initial string
		    tempStr = malloc((sizeof(char)*strLength)+1);

	            //initialize a counter that will go through the string.
		    int ctr = 1;
		    
		    //copy the first string
		    strcpy(tempStr,tempData[0]);
		    
	            //concatenate subsequent strings.
		    while(tempData[ctr]!=NULL){
			strcat(tempStr,spaceConst);
			strcat(tempStr,tempData[ctr]);
			ctr++;
		    }
		   printf("%s\t\t%d\n",tempStr,counter);
		   free(tempStr);
		}
		temp = temp -> next;
		counter++;
	}
	printf("\n");
	return 1;
}

int fg(char * arg){
	if(arg ==NULL){
		printf("Must provide an argument to fg\n");
		return -1;
	}
	if(!isNumber(arg)){
	    printf("Must provide an integer value\n");
	    return -1;
	}
	int command_arg = atoi(arg);

	if(command_arg >=1){
		Node * temp = getNodeData(command_arg);
		if(temp!=NULL){
		    //sets saved command to the foreground
		    temp -> bg = 0;
		    return 1;
		}
		printf("Out of range for process id's.\n");
		return -1;
	}
	printf("Process ID must be >= 1\n");
	return -1;
}

//helper function to check to see if a function is a background
//function. 


int getcmd(char *prompt, char *args[], int *background)
{
    int length, i = 0;
    char *token, *loc;
    char *line;
    size_t linecap = 0;
    
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0) {
        exit(-1);
    }

    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;

    while ((token = strsep(&line, " \t\n")) != NULL) {
        int j;
	for (j = 0; j < strlen(token); j++){
            if (token[j] <= 32){
                token[j] = '\0';
		}
	}
        if (strlen(token) > 0){
            args[i++] = token;
	}
    }
	args[i++] = NULL;

    return i;
}

