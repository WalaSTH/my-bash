#include <string.h>  
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "builtin.h"
#include "syscall_mock.h"
#include "./tests/syscall_mock.h"

bool builtin_is_exit(pipeline pipe){
    assert(pipe != NULL);
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_front(caux),"exit");
	return res == 0;

}
bool builtin_is_cd(pipeline pipe){
    assert(pipe != NULL);
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_front(caux),"cd");
	return res == 0;
}

bool builtin_is_internal(pipeline pipe){
    assert(pipe != NULL);    
	return builtin_is_exit(pipe) || builtin_is_cd(pipe);
}

void builtin_exec(pipeline pipe){
    assert(builtin_is_internal(pipe));
    if(builtin_is_cd(pipe)){
        scommand com = pipeline_front(pipe);
        if(!scommand_is_empty(com)){
            scommand_pop_front(com);
            char* path = scommand_front(com);
            int success = chdir(path);
            if(success == -1){
                printf("No se pudo acceder al directorio.\n");
            }
        }
        else{
            printf("Ingrese un directorio\n");
        }
    }
}


