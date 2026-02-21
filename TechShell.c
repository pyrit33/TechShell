// Name: Cheyenne Deloney
// Descrption: A bash-like shell with similar functionality 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_CMD_SIZE 256
#define MAX_ARG_SIZE 16
#define NUM_SMPL_CMDS 3

// functional prototyping
void displayPrompt();
char* getInput();
void executeSimpleCommand(char* command);
char** parseInput(char* input, char* args[]);
void executeCommand(char** command);

int main(){
    char* input;
    char* args[MAX_ARG_SIZE];

    // initial prompt (only displays once)
    printf("Techshell:\nEnter 'exit' to quit or 'help' for more valid commands.\n");

    for(;;){
        // display the prompt
        displayPrompt();

        // get the user's input
        input = getInput();

        // exit early if it's the command
        if (strcmp(input, "exit\n") == 0){
            break;
        }

        // ignore blank spaces
        if (strcmp(input, "\n") == 0){
            continue;
        }

        // remove enter at the end of the input
        input[strcspn(input, "\n")] = '\0';

        // simple commands taken care of
        if (strcmp(input, "cd") == 0 || strcmp(input, "pwd") == 0 || strcmp(input, "help") == 0)){
            executeSimpleCommand(input);
            continue;
        }

        // parse the command line
        parseInput(input, args);

        // execute the command
        executeCommand(args);
    }
    
    exit(0);
}

// function to display the prompt (contains username, hostname, and cwd)
void displayPrompt(){
    char cwd[MAX_CMD_SIZE];
    char* username = getenv("USER");
    char* hostname;
    if (getenv("HOST") != NULL){
        hostname = getenv("HOST");
    } else {
        hostname = username;
    }
    printf("%s@%s:%s$ ", username, hostname, getcwd(cwd, sizeof(cwd)));
}

// function that returns the user's input
char* getInput(){
    char* buffer = malloc(MAX_CMD_SIZE);
    char* input = fgets(buffer, sizeof(buffer), stdin);
    return input;
}

// for early access to simple commands before parsing
// takes the user's input and completes the command (if it's one of the simple commands)
void executeSimpleCommand(char* command){
    char* SimpleCommands[MAX_SMPL_CMDS];
    int switchCounter = 0;

    // array with simple commands
    SimpleCommands[0] = "pwd";
    SimpleCommands[1] = "help";
    SimpleCommands[2] = "cd";

    // finds command that matches input
    for (int i = 0; i < NUM_SMPL_CMDS; i++){
        if (strcmp(command, SimpleCommands[i]) == 0){
            switchCounter = i + 1;
            break;
        }
    }

    // completes the command using switchcase
    // if error, displays message
    switch (switchCounter){
        case 1: // pwd
            char cwd[MAX_CMD_SIZE];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
            break;
        case 2: // help
            printf("Valid commands: 'pwd' 'cd', 'ls' 'ls -l', etc");
            break;
        case 3: // cd
            chdir(getenv("HOME"));
            break;
        default:
            printf("Command '%s' not found\nTry using 'help' for a list of valid commands.\n", command);
            break;
    }
}

// parses the input into an array to handle commands that are not simple commands
char** parseInput(char* input, char* args[]){
    int i = 0;
    char* token;

    // geting rid of enter
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n'){
        input[len - 1] = '\0';
    }

    // tokenize input by spaces
    token = strtok(input, " ");
    while (token != NULL && i < MAX_ARG_SIZE - 1){
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    args[i] = NULL;

    return args;
}

// executes all over commands using execvp
// i was unable to implement redirection
void executeCommand(char** args){
    pid_t pid = fork();
    
    if (pid == -1){
        perror("Failed to create a child.\n");
        return;
    } else if (pid == 0) { // child
        if (execvp(args[0], args) == -1){
            perror("Command couldn't execute.\n");
        }
        exit(0);
    } else if (pid > 0) { // parent
        wait(NULL);
    } else {
        perror("Failed to fork.\n");
    }

}
