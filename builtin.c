#include <string.h>  //Calculo que respecto a esto no hay problemas (no estaba)
#include "builtin.h"

bool builtin_is_exit(pipeline pipe){
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_to_string(caux),"exit");
	return res == 0;

}
bool builtin_is_cd(pipeline pipe){
	scommand caux = pipeline_front(pipe); 
	int res = strcmp(scommand_to_string(caux),"cd");
	return res == 0;
}

bool builtin_is_internal(pipeline pipe){
	return builtin_is_exit(pipe) || builtin_is_cd(pipe);
}

void builtin_exec(pipeline pipe){
}


