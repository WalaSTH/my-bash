#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>
#include "command.h"
#include "execute.h"
#include "builtin.h"
#define READ 0
#define WRITE 1

void execute_pipeline(pipeline apipe){
    assert(apipe!=NULL);
    if(builtin_is_internal(apipe)){
        builtin_exec(apipe);
    }
    else{
        scommand scom1, scom2;
        unsigned int n_commands = pipeline_length(apipe);
        scom1 = pipeline_front(apipe);
        pipeline_pop_front(apipe);
        scom2 = pipeline_front(apipe);
        char *com_arr1[100], *com_arr2[100];
        bool multi = n_commands > 1;
        int fd[2];
        pipe(fd);

        int pid1 = fork();
        if(pid1 == 0){
            //Child 1            
            unsigned int n = scommand_length(scom1);
            for(unsigned int i = 0u; i < n; ++i){
                com_arr1[i] = scommand_front(scom1);
                scommand_pop_front(scom1);
            }
            com_arr1[n] = NULL;
            char* fileIn =  scommand_get_redir_in(scom1);
            if(fileIn != NULL){ //Check if we have a redir out
                int f = open(fileIn,O_RDONLY | O_CREAT,0777);
                dup2(f,STDIN_FILENO);
            }
            if(multi){
                close(fd[READ]);
                dup2(fd[WRITE], STDOUT_FILENO);
                close(fd[WRITE]);
            }
            execvp(com_arr1[0], com_arr1);
        }
        else{
            int pid2 = 1;
            if(multi){
                pid2 = fork();
            }
            if (pid2 == 0){
                //Child 2
                close(fd[WRITE]);
                unsigned int n2 = scommand_length(scom2);
                for(unsigned int i = 0u; i < n2; ++i){
                    com_arr2[i] = scommand_front(scom2);
                    scommand_pop_front(scom2);
                }
                com_arr2[n2] = NULL;
                dup2(fd[READ],STDIN_FILENO);
                char* fileOut = scommand_get_redir_out(scom2);
                if(fileOut != NULL){ //Check if we have a redir out
                    int f = open(fileOut,O_WRONLY | O_CREAT,0777);
                    dup2(f,STDOUT_FILENO);
                }
                close(fd[READ]);
                execvp(com_arr2[0], com_arr2);
            }
            else{
                //Main process
                close(fd[0]);
                close(fd[1]);
                waitpid(pid1, NULL, 0);
                if(multi){
                    waitpid(pid2, NULL, 0);
                }
            }
        }
    }
}