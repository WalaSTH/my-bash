#include <stdio.h> 
#include <glib.h>

#include "command.h"


/********** COMANDO SIMPLE **********/

/* Estructura correspondiente a un comando simple.
 * Es una 3-upla del tipo ([char*], char* , char*).
 */

struct scommand_s {
    GSList *args;
    char * redir_in;
    char * redir_out;
};


scommand scommand_new(void){
    scommand new = malloc(sizeof(struct scommand_s));
    new->args=NULL;//ESTO ES NUEVO
    new->redir_in = NULL;
    new->redir_out = NULL;
    return new;    
}

scommand scommand_destroy(scommand self){
	if(self != NULL){
        g_slist_free(self->args);
        free(self->redir_in);
        free(self->redir_out);
    }
    free(self);
    self = NULL;
    return self;
}

void scommand_push_back(scommand self, char * argument){
    if (self->args == NULL){
        self->args = g_slist_alloc();
        self->args = g_slist_append(self->args, argument);
    }
    else {
        self->args = g_slist_append(self->args, argument);
    }
}

void scommand_pop_front(scommand self){
    self->args = g_slist_delete_link(self->args, g_slist_nth(self->args,0));
}
void scommand_set_redir_in(scommand self, char * filename){
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    self->redir_out = filename;
}

bool scommand_is_empty(const scommand self){
    return self->args == NULL;
}

unsigned int scommand_length(const scommand self){
    return g_slist_length(self->args);
}

char * scommand_front(const scommand self){
    return (char*) g_slist_nth(self->args,0);
}

char * scommand_get_redir_in(const scommand self){
    return self->redir_in;
}

char * scommand_get_redir_out(const scommand self){
    return self->redir_out;
}

char * scommand_to_string(const scommand self){
	return (char*) g_slist_nth(self->args,0); //NO ESTA COMPLETO
}





/********** COMANDO PIPELINE **********/

/* Estructura correspondiente a un comando pipeline.
 * Es un 2-upla del tipo ([scommand], bool)
 */

struct pipeline_s {
    GSList *scmds;
    bool wait;
};



pipeline pipeline_new(void){
    pipeline new = malloc(sizeof(struct pipeline_s));
    new->scmds = NULL;
    new->wait = true;
	return new;
}

pipeline pipeline_destroy(pipeline self){
    //self->scmds = for i = 0 to length free each scommand and then
    g_slist_free(self->scmds);
    free(self);
	return self;
}

void pipeline_push_back(pipeline self, scommand sc){
    if(self->scmds == NULL){
        self->scmds = g_slist_alloc();
        self->scmds = g_slist_append(self->scmds, sc);  
    }
    else{
        self->scmds = g_slist_append(self->scmds, sc);
    }
}

void pipeline_pop_front(pipeline self){
    self->scmds = g_slist_delete_link(self->scmds, g_slist_nth(self->scmds,0));
}

void pipeline_set_wait(pipeline self, const bool w){
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
	return self->scmds == NULL;
}

unsigned int pipeline_length(const pipeline self){    
    return g_slist_length(self->scmds);
}

scommand pipeline_front(const pipeline self){
	return (scommand) g_slist_nth(self->scmds,0);
}

bool pipeline_get_wait(const pipeline self){
	return self->wait;
}

char * pipeline_to_string(const pipeline self){
	return scommand_to_string((scommand)g_slist_nth(self->scmds,0));
}


