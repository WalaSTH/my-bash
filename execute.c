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
/*create_child -> Creates a child process that wild execute a command
number-> indicates the position of the command on the pipeline
elements-> indicates the total commands to execute in the pipeline
fd[] -> the pipe
scom -> the scomand to execute
*/
static pid_t create_child(unsigned int number, unsigned int elements,int fd[], scommand scom){
    pid_t pid = fork();
    char* a[100];
    if(pid == -1){
        printf("Error while forking sub-process\n");
    }
    else if(pid == 0){
        unsigned int size = scommand_length(scom);
        fill_array(a, size, scom);
        char* fileIn =  scommand_get_redir_in(scom);
        if(fileIn != NULL){ //Check if we have a redir in
            int f = open(fileIn,O_RDONLY | O_CREAT,0777);
            dup2(f,STDIN_FILENO);
        }
        char* fileOut =  scommand_get_redir_in(scom);
        if(fileOut != NULL){ //Check if we have a redir out
            int f = open(fileOut,O_WRONLY | O_CREAT,0777);
            dup2(f,STDOUT_FILENO);
        }
        if(number == 1 && number == elements){
            //Single command case
            //Will not read from nor write to pipe
        }
        else if(number == 1){
            //First of many commands case
            //Wil only write to pipe
            dup2(fd[WRITE], STDOUT_FILENO);
        }
        else if(number == elements){
            //Last of many commands case
            //Will only read from pipe
            dup2(fd[READ],STDIN_FILENO);
        }
        else if (number != 1 && number != elements){
            //In the middle of commands case
            //Will both read from and write to pipe
            dup2(fd[READ],STDIN_FILENO);            
            dup2(fd[WRITE], STDOUT_FILENO);
            
        }
        close(fd[READ]);
        close(fd[WRITE]);
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
        unsigned int n_commands = pipeline_length(apipe);
        int fd[2];
        pid_t pids[100];
        pipe(fd);
        for(unsigned int i = 1u; i<= n_commands; ++i){
            scommand scom = pipeline_front(apipe);
            pipeline_pop_front(apipe);
            pids[i-1u] = create_child(i, n_commands, fd, scom);
        }
        close(fd[0]);
        close(fd[1]);
        if(pipeline_get_wait(apipe)){
            for(unsigned int i = 0; i < n_commands; ++i){
                waitpid(pids[i], NULL, 0);
            }
        }

    }
}
 


