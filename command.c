#include <stdio.h> 
#include <glib.h>
#include <assert.h>

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
	assert(new != NULL && scommand_is_empty (new) &&
     		scommand_get_redir_in (new) == NULL &&
    		scommand_get_redir_out (new) == NULL);     
	return new;    
}

scommand scommand_destroy(scommand self){
	if(self != NULL){
		if(self->args == NULL){
			g_slist_free(self->args);
		}
		if(self->redir_in == NULL){
			free(self->redir_in);
		}
		if(self->redir_out == NULL){
			free(self->redir_out);
		}
		free(self);    
		self = NULL;
	}
	assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
	assert(self != NULL && argument!= NULL);
    if(self->args == NULL){
        self->args = g_slist_alloc();
	}
    self->args = g_slist_append(self->args, argument);
	assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
	assert(self != NULL);
    self->args = g_slist_delete_link(self->args, g_slist_nth(self->args,0)); //SE DEBE PROBAR 
	assert(!scommand_is_empty(self));
}
void scommand_set_redir_in(scommand self, char * filename){
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    self->redir_out = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self != NULL);
	return self->args == NULL;
}

unsigned int scommand_length(const scommand self){
    return (unsigned int)g_slist_length(self->args);
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

char* scommand_to_string(GSList *l){
    //return (char*)g_slist_nth(self->args,0); //NO ESTA COMPLETO
    char* res = calloc(1,sizeof(char));
    char** aux;
    unsigned int n = (unsigned int)g_slist_length(l);
    aux = (char**)g_slist_nth(l,0);
    res = strmerge(res,*aux);
    for(unsigned int i = 1; i < n; ++i){
        aux = (char**)g_slist_nth(l,i);
        res = strmerge(res, " ");
        res = strmerge(res,*aux);
    }
    return res;
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


