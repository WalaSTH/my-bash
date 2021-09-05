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
        //Vamos a ejecutar 2 scommands conectados por un pipeline
        // y dirigiendo a un archivo 
        int pipeline[2];
        int pid1, pid2;
        unsigned int n;
        char *com_array1[100], *com_array2[100]; //VER SI USAR 1
        scommand com1, com2;
        if (pipe(pipeline) == -1){
            printf("Pipe failed");
            //Habria que volver al myBash
        }
        pid1 = fork();
        if(pid1 == 0){
            //First process
            close(pipeline[READ]);
            com1 = pipeline_front(apipe);
            pipeline_pop_front(apipe); //Sera un data? !MIEDO! CUIDADO!
            n = scommand_length(com1);
            for(unsigned int i = 0u; i < n; ++i){
                com_array1[i] = scommand_front(com1);
                scommand_pop_front(com1);
            }
            com_array1[n] = NULL;
            dup2(pipeline[WRITE], STDOUT_FILENO);
            close(pipeline[WRITE]);
            execvp(com_array1[0], com_array1);
        }
        waitpid(pid1, NULL, 0); //Revisar-> el &
        pid2 = fork();
        if(pid2 == 0){
            //Second process
            close(pipeline[WRITE]);
            //aca tal vez if? tenemos que ver las redirecciones
            dup2(pipeline[READ], STDIN_FILENO);
            close(pipeline[READ]);
            //Modularizable
            com2 = pipeline_front(apipe);
            n = scommand_length(com2);
            for(unsigned int i = 0u; i < n; ++i){
                com_array2[i] = scommand_front(com2);
                scommand_pop_front(com2);
            }
            com_array2[n] = NULL;
            execvp(com_array2[0], com_array2);
        }
        waitpid(pid2, NULL, 0);
        close(pipeline[READ]);
        close(pipeline[WRITE]);
    }
}
