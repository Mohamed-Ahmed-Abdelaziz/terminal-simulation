#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

FILE *fp;
int back_ground_process_id = 0;

char input_buffer[50];
char exportValues[200] = "";
char exportValuesBuffer[200] = "";
char *arguments[10];
char input[30];
char *arguments2[] = {"ls","-l", NULL};

void parse_input(){
    gets(input);
}
void write_to_log_file(){
        fp = fopen("log.txt", "a");
        if(fp == NULL){
            printf("there is a problem\n");
            return;
        }
        fprintf(fp, "child process terminated\n");
        fclose(fp);
}
void reap_child_zombie(){
    waitpid(back_ground_process_id, NULL, 0);
}
void setup_environment(){
    chdir("/home/mohamed");
}
void on_child_exit(int sig){
    if(back_ground_process_id != 0){
        reap_child_zombie();
        write_to_log_file();
        back_ground_process_id = 0;
    }else{
        write_to_log_file();
    }
}

int main()
{
    setup_environment();    // changing directory to home

    signal(SIGCHLD, on_child_exit);     // reaping and logging backgroung processes

    do{
        parse_input();      // taking input from user


        // evaluating-expression
        strcpy(input_buffer, input);
        int i = 0;
        char *piece = strtok(input, " ");
        arguments[i++] = piece;
        while(piece != NULL){
            piece = strtok(NULL, " ");
            arguments[i++] = piece;
        }

        if(strcmp(arguments[0], "exit") == 0) return;

        // dealing with built-in commands
        if(strcmp(arguments[0], "export") == 0){
                strcat(exportValues, input_buffer);
                strcat(exportValues, "\n");
                strcpy(exportValuesBuffer, exportValues);
                continue;
            }
        else if(strchr(input_buffer, '$')){
            strcat(exportValues, "\n");
            strcat(exportValues, input_buffer);
            system(exportValues);
            strcpy(exportValues, exportValuesBuffer);
            continue;
        }
        // forking child processes
        int pid = fork();
        if(pid == 0){
            if(i == 1) return 0;

            if(strcmp(arguments[0], "mkdir") == 0){
                mkdir(arguments[1]);
            }
            else if(strcmp(arguments[0], "cd") == 0){
                if(strcmp(arguments[1], "~") == 0) chdir("/home/mohamed");
                else chdir(arguments[1]);
            }
            else{
                // dealing with executable or error
                execvp(arguments[0], arguments);
                if(strcmp(arguments[0], "exit") != 0)
                    printf("invalid input\n");
            }
            return 0;
        }else{
            if(i > 2){
                if(strcmp(arguments[1], "&") != 0)
                    waitpid(pid, NULL, 0);
                else back_ground_process_id = pid;
            }else{
                waitpid(pid, NULL, 0);
            }
        }
    }while(1);

    return 0;
}
