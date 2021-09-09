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

static void fill_array(char* a[], unsigned int size, scommand scom){
    for(unsigned int i = 0u; i < size; ++i){
        a[i] = scommand_front(scom);
        scommand_pop_front(scom);
    }
    a[size] = NULL;    
}
/*create_child_process -> Creates a child process that wild execute a command
fd[] -> the pipe to write to
prev -> the previous pipe to read from
last -> indicates if we are executing the last process in pipeline
scom -> the scomand to execute
*/
static pid_t create_child_process(int fd[], int prev,bool last, scommand scom){
    pid_t pid = fork();
    unsigned int n = scommand_length(scom);
    char* a[100];
    if(pid == -1){
        printf("Error while forking sub-process\n");
    }
    else if(pid == 0){
        //Child process
        if (prev != STDIN_FILENO){ 
            //Not first command case, will read from pipe
            dup2(prev,STDIN_FILENO);
            close(prev);
        }
        if(!last){
            //In the middle commands will write to pipe
            dup2(fd[WRITE],STDOUT_FILENO);
            close(fd[WRITE]);
        }
        char* fileIn =  scommand_get_redir_in(scom);
        if(fileIn != NULL){ //Check if we have a redir in
            int f = open(fileIn,O_RDONLY | O_CREAT,0777);
            dup2(f,STDIN_FILENO);
            close(f);
        }
        char* fileOut =  scommand_get_redir_out(scom);
        if(fileOut != NULL){ //Check if we have a redir out
            int f = open(fileOut,O_WRONLY | O_CREAT,0777);
            dup2(f,STDOUT_FILENO);
            close(f);
        }
        fill_array(a,n,scom);
        int exec = execvp(a[0],a);
        if(exec == -1){
            printf("Error executing process '%s'\n", a[0]);
            _exit(1);
        }
    }
    return pid;
}

void execute_pipeline(pipeline apipe){
    assert(apipe!=NULL);
    if(builtin_is_internal(apipe)){
        builtin_exec(apipe);
    }
    else{
        scommand scom;
        unsigned int n_commands = pipeline_length(apipe);
        int prev_pipe, fd[2];
        pid_t pids[100];
        prev_pipe = STDIN_FILENO;
        //Executing all commands in pipeline
        for(unsigned int i = 0u; i< n_commands; ++i){
            bool last = i+1u == n_commands;
            scom = pipeline_front(apipe);
            pipeline_pop_front(apipe);
            pipe(fd);
            pids[i] = create_child_process(fd, prev_pipe, last, scom);
            //closing files we don't need on parent process
            close(prev_pipe);
            close(fd[WRITE]);
            prev_pipe = fd[READ];
        }
        //Restoring file descriptors
        prev_pipe = STDERR_FILENO;
        dup2(prev_pipe, STDIN_FILENO);
        //close(prev_pipe) -> Doing this will create a weird bug where on the 
        //second run of mybash it closes ??? ask teachers

        //Wait
        if(pipeline_get_wait(apipe)){
            for(unsigned int i = 0; i < n_commands; ++i){
                waitpid(pids[i], NULL, 0);
            }
        }

    }
}