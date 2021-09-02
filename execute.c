#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "command.h"
#include "execute.h"
#include "builtin.h"

void execute_pipeline(pipeline apipe){
    if(builtin_is_internal(apipe)){
        builtin_exec(apipe);
    }
    else{
        scommand com = pipeline_front(apipe);
        char *command = scommand_front(com);
        //const unsigned int s_size = scommand_length(com);
        char *args[4];
        args[0] = command;
        args[1] = NULL;
        int pid = fork();
        if(pid == 0){
            int exec = execvp(command, args);  
            if(exec == -1){
                printf("Comando inválido.\n");
            }
        }
        else{
            if(pipeline_get_wait(apipe)){
                waitpid(pid, NULL, 0);
            }
        }      
    }
}
