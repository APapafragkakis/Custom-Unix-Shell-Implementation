#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 500
int f_descriptor;

void closeLastArgument(char* args[], int* arg_number, int* index_of_arg, int* i){
    if(*index_of_arg > 0){
        args[*arg_number][*index_of_arg] = '\0';
        (*arg_number)++;
        *index_of_arg = 0;
    }
}

void execute(char* args[], int arg_number){
    #ifdef DEBUG
        fprintf(stderr, "Executing Command: %s\nArgs: ", args[arg_number]);
        for(int i = arg_number + 1; args[i] != NULL; i++){
            fprintf(stderr, "%s ", args[i]);
        }
        fprintf(stderr, "\n");
    #endif

    execvp(args[arg_number], args + arg_number);
}

void Do(char* input)
{
    char **args;

    int index_of_arg = 0;
	int i = 0;
    int arg_number = 0;
    int number_of_pipes = 0;
	
    args=(char**)malloc(sizeof(char*) * SIZE);

    // Skip and leading spaces
    while(input[i] == ' ') i++;

    while(input[i] != '\0') {

        if(index_of_arg == 0){
            args[arg_number] = (char *)malloc(sizeof(char) * 100);
        }

        int isQuote = input[i] == '\"';
        int isRedirection = input[i] == '<' || input[i] == '>' || (input[i] == '>' && input[i+1] == '>');
        int isSpace  = input[i] == ' ';
        int isEquals = input[i] == '=';
        int isPipe = input[i] == '|';

        if (isQuote)
        {
            i++; // Skip the quote

            while(input[i] != '\"'){ // Read until the next quote
                args[arg_number][index_of_arg] = input[i];
                index_of_arg++;
                i++;
            }

            index_of_arg++;

            closeLastArgument(args, &arg_number, &index_of_arg, &i);

            i++; // Skip the quote
            continue;
        }


        if(isSpace){
            if(index_of_arg == 0){
                i++;
                continue;
            }

            closeLastArgument(args, &arg_number, &index_of_arg, &i);

            i++; // Skip the space

            continue;
        }


        if(isEquals){
            // If there is not space between the variable and the equals sign, complete the variable name
            closeLastArgument(args, &arg_number, &index_of_arg, &i);

            args[arg_number] = (char *)malloc(sizeof(char) * 2);
            args[arg_number][0] = '=';
            args[arg_number][1] = '\0';

            // Swap the variable name with the equals sign so we know it is an equals before we execute the command
            char* temp = args[arg_number];
            args[arg_number] = args[arg_number-1];
            args[arg_number-1] = temp;

            arg_number++;

            // Skip the equals
            i++;

            continue;
        }

        if(isPipe){
            closeLastArgument(args, &arg_number, &index_of_arg, &i);

            args[arg_number] = (char *)malloc(sizeof(char) * 2);
            args[arg_number][0] = '|';
            args[arg_number][1] = '\0';

            number_of_pipes++;

            arg_number++;
            i++;
            continue;
        }

        if(isRedirection){
            closeLastArgument(args, &arg_number, &index_of_arg, &i);

            args[arg_number] = (char *)malloc(sizeof(char) * (2 + (input[i+1] == '>')));
            args[arg_number][0] = input[i];

            if(input[i+1] == '>'){
                args[arg_number][1] = '>';
                args[arg_number][2] = '\0';
                i++;
            }else{
                args[arg_number][1] = '\0';
            }

            arg_number++;
            i++;
            continue;
        }

        args[arg_number][index_of_arg] = input[i];
        index_of_arg++;
        i++;
	}

    if(index_of_arg > 0){
        args[arg_number][index_of_arg] = '\0';
        arg_number++;
    }

    args[arg_number] = NULL;

    if(number_of_pipes == 0){
        if(strcmp(args[0], "exit") == 0){
            exit(0);
        }

        for(int i = 0; args[i] != NULL; i++){
            // Check if the argument starts with a dollar sign
            if(args[i][0] == '$'){
                char* env = getenv(args[i] + 1);
                if(env != NULL){
                    strcpy(args[i], env);
                } else {
                    strcpy(args[i], "");
                }
            }
        }

        if(strcmp(args[0], "=") == 0){
            setenv(args[1], args[2], 1);
        }
        else if(strcmp(args[0], "cd") == 0){
            if(chdir(args[1]) == -1){
                printf("Error: Directory not found\n");
            }
        } 
        else {
            // Search for redirection
            int input_fd = 0, output_fd = 1;
            for(int i = 0; args[i] != NULL; i++){
                if(strcmp(args[i], "<") == 0){
                    input_fd = open(args[i+1], O_RDONLY);
                    if(input_fd == -1){
                        perror("Error opening input file");
                        return;
                    }
                    args[i] = NULL;
                    args[i+1] = NULL;
                } else if(strcmp(args[i], ">") == 0){
                    output_fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if(output_fd == -1){
                        perror("Error opening output file");
                        return;
                    }
                    args[i] = NULL;
                    args[i+1] = NULL;
                } else if(strcmp(args[i], ">>") == 0){
                    output_fd = open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if(output_fd == -1){
                        perror("Error opening output file");
                        return;
                    }
                    args[i] = NULL;
                    args[i+1] = NULL;
                }
            }

            if(fork() == 0){
                if(input_fd != 0){
                    dup2(input_fd, 0);
                    close(input_fd);
                }
                if(output_fd != 1){
                    dup2(output_fd, 1);
                    close(output_fd);
                }
                execute(args, 0);
            } else {
                waitpid(-1, NULL, 0);
                if(input_fd != 0) close(input_fd);
                if(output_fd != 1) close(output_fd);
            }
        }
    } else {
        int num_commands = number_of_pipes + 1;

        int (*pipes)[2] = malloc(sizeof(int[2]) * number_of_pipes);

        for(int i = 0; i < number_of_pipes; i++) {
            pipe(pipes[i]);
        }

        int cmd_start = 0;
        pid_t pid;
        for(int i = 0; i < num_commands; i++) {
            int cmd_end = cmd_start;
            while(args[cmd_end] != NULL && strcmp(args[cmd_end], "|") != 0){
                cmd_end++;
            }

            args[cmd_end] = NULL;

            pid = fork();

            if(pid == 0){
                if(i > 0){
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }

                if(i < number_of_pipes){
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                for(int j = 0; j < number_of_pipes; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                for(int k = cmd_start; args[k] != NULL; k++){
                    if(args[k][0] == '$'){
                        char* env = getenv(args[k] + 1);
                        if(env != NULL){
                            args[k] = env;
                        } else {
                            args[k] = "";
                        }
                    }
                }

                if(strcmp(args[cmd_start], "cd") == 0){
                    if(args[cmd_start + 1] != NULL){
                        chdir(args[cmd_start + 1]);
                    }
                    exit(0); // Exit child after handling built-in
                }
                else if(strcmp(args[cmd_start], "exit") == 0){
                    exit(0);
                }
                else if(strcmp(args[cmd_start], "=") == 0){
                    if(args[cmd_start + 1] != NULL && args[cmd_start + 2] != NULL){
                        setenv(args[cmd_start + 1], args[cmd_start + 2], 1);
                    }
                    exit(0);
                }

                int input_fd = 0, output_fd = 1;
                for(int k = cmd_start; args[k] != NULL; k++){
                    if(strcmp(args[k], "<") == 0){
                        if(args[k+1] == NULL){
                            fprintf(stderr, "Syntax error: expected filename after '<'\n");
                            exit(EXIT_FAILURE);
                        }
                        input_fd = open(args[k+1], O_RDONLY);
                        if(input_fd == -1){
                            perror("Error opening input file");
                            exit(EXIT_FAILURE);
                        }
                        args[k] = NULL;
                        args[k+1] = NULL;
                    }
                    else if(strcmp(args[k], ">") == 0){
                        if(args[k+1] == NULL){
                            fprintf(stderr, "Syntax error: expected filename after '>'\n");
                            exit(EXIT_FAILURE);
                        }
                        output_fd = open(args[k+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if(output_fd == -1){
                            perror("Error opening output file");
                            exit(EXIT_FAILURE);
                        }
                        args[k] = NULL;
                        args[k+1] = NULL;
                    }
                    else if(strcmp(args[k], ">>") == 0){
                        if(args[k+1] == NULL){
                            fprintf(stderr, "Syntax error: expected filename after '>>'\n");
                            exit(EXIT_FAILURE);
                        }
                        output_fd = open(args[k+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if(output_fd == -1){
                            perror("Error opening output file");
                            exit(EXIT_FAILURE);
                        }
                        args[k] = NULL;
                        args[k+1] = NULL;
                    }
                }

                if(input_fd != 0){
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }

                if(output_fd != 1){
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }

                execute(args, cmd_start);
            }

            cmd_start = cmd_end + 1;
        }

        for(int i = 0; i < number_of_pipes; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for(int i = 0; i < num_commands; i++) {
            wait(NULL);
        }

        free(pipes);
    }

    for(int i = 0; i < arg_number; i++){
        free(args[i]);
    }
    free(args);     
}

void yellow() {
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}


int main() {
    char cwd[1024];
    char input[1000];
    char *user; 

    char* token;
    char* delimeterTokens = ";\n";
    f_descriptor = 0;

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            strcpy(cwd, "unknown_dir");
        }

        user = getlogin();
        if (user == NULL) {
            user = malloc(15);
            strcpy(user, "unknown_user");
        }

        yellow();
        printf("csd5084-hy345sh@%s:%s$ ", user, cwd);
        reset();

        fgets(input, 1000, stdin);

        token = strtok(input, delimeterTokens);

        while (token != NULL)
        {
            Do(token);

            token = strtok(NULL, delimeterTokens);
        }
    }

    return 0;
}
