#include <string.h>  //Calculo que respecto a esto no hay problemas (no estaba)
#include <unistd.h>
#include <stdio.h>
#include "builtin.h"

bool builtin_is_exit(pipeline pipe){
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_front(caux),"exit");
	return res == 0;

}
bool builtin_is_cd(pipeline pipe){
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_front(caux),"cd");
	return res == 0;
}

bool builtin_is_internal(pipeline pipe){
	return builtin_is_exit(pipe) || builtin_is_cd(pipe);
}

void builtin_exec(pipeline pipe){
    if(builtin_is_cd(pipe)){
        scommand com = pipeline_front(pipe);
        if(!scommand_is_empty(com)){
            scommand_pop_front(com);
            char* path = scommand_front(com);
            int success = chdir(path);
            if(success == 0){
                printf("Estas en el directorio %s\n", path);
            }
            else{
                printf("No se pudo acceder al directorio.\n");
            }
        }
        else{
            printf("Ingrese un directorio\n");
        }
    }
}


