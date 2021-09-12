#include <stdio.h> 
#include <glib.h>
#include <assert.h>

#include "strextra.h"
#include "command.h"
#include "tests/syscall_mock.h"

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
    if(new == NULL){
        fprintf(stderr,"Fatal: Error to allocate %zu bytes.\n",sizeof(struct scommand_s));
        exit(EXIT_FAILURE);
    }
    else{
        new->args=NULL;//ESTO ES NUEVO
        new->redir_in = NULL;
        new->redir_out = NULL;
	    assert(new != NULL && scommand_is_empty (new) &&
         		scommand_get_redir_in (new) == NULL &&
        		scommand_get_redir_out (new) == NULL);   
    }
  
	return new;    
}

scommand scommand_destroy(scommand self){
	if(self != NULL){
		if(self->args != NULL){
			g_slist_free_full(self->args,free);
            self->args = NULL;
		}
		if(self->redir_in != NULL){
			free(self->redir_in);
            self->redir_in = NULL;
		}
		if(self->redir_out != NULL){
			free(self->redir_out);
            self->redir_out = NULL;
		}
		free(self);    
		self = NULL;
	}
	assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
	assert(self != NULL && argument!= NULL);
    self->args = g_slist_append(self->args, argument);
	assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
	assert(self != NULL && !scommand_is_empty(self));
    free((char*)g_slist_nth_data(self->args,0));
    self->args = g_slist_delete_link(self->args, g_slist_nth(self->args,0)); //SE DEBE PROBAR 
}
void scommand_set_redir_in(scommand self, char * filename){
    assert(self != NULL);
    if(self->redir_in != NULL){
        free(self->redir_in);
    }
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self != NULL);
    if(self->redir_out != NULL){
        free(self->redir_out);
    }    
    self->redir_out = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self != NULL);
	return (g_slist_length(self->args) == 0);
}

unsigned int scommand_length(const scommand self){
    assert(self != NULL );
    unsigned int res = (unsigned int)g_slist_length(self->args);  
    assert((res == 0) == scommand_is_empty(self));
    return res;
    }

char * scommand_front(const scommand self){
    assert(self != NULL && !scommand_is_empty(self));
    char* res = (char*)g_slist_nth_data(self->args,0);
    assert(res != NULL);
    return res;
}

char * scommand_get_redir_in(const scommand self){
    assert(self!=NULL);
    return self->redir_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);
    return self->redir_out;
}

char* scommand_to_string(scommand self){
	char *res,*resspace,*argument,*aux;;
	unsigned int cant = (unsigned int)g_slist_length(self->args);
    res = strdup(scommand_front(self));
    for(unsigned int i = 1u; i < cant; i++){
	    resspace = strmerge(res, " ");
	    free(res);
        argument=(char*)g_slist_nth_data(self->args, i);
        aux = strmerge(resspace, argument);
        free(resspace);
        res=aux;
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
    if(new == NULL){
        fprintf(stderr,"Fatal: Error to allocate %zu bytes.\n",sizeof(struct pipeline_s));
        exit(EXIT_FAILURE);
    }
    else{
        new->scmds = NULL;
        new->wait = true;
    }
    assert(new != NULL && pipeline_is_empty(new));
	return new;
}

pipeline pipeline_destroy(pipeline self){
    while(self->scmds != NULL){
        pipeline_pop_front(self);
    }
    free(self);
    self = NULL;
	return self;
}

void pipeline_push_back(pipeline self, scommand sc){
    assert(self != NULL && sc != NULL);
    self->scmds = g_slist_append(self->scmds, sc); 
    assert(!pipeline_is_empty(self)); 
}

void pipeline_pop_front(pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));
    scommand_destroy(g_slist_nth_data(self->scmds,0));
    self->scmds = g_slist_delete_link(self->scmds,g_slist_nth(self->scmds,0));
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self != NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
	assert(self != NULL);
    return (g_slist_length(self->scmds) == 0);
}

unsigned int pipeline_length(const pipeline self){    
    assert(self != NULL);
    unsigned int res = (unsigned int)g_slist_length(self->scmds);
    assert((res==0) == pipeline_is_empty(self));
    return res; 
    }

scommand pipeline_front(const pipeline self){
	assert(self != NULL && !pipeline_is_empty(self));
    scommand c = (scommand)g_slist_nth_data(self->scmds,0);
    assert(c != NULL);
    return c;
}

bool pipeline_get_wait(const pipeline self){
	assert(self != NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self){
    char *res, *aux;
    scommand scom = (scommand)g_slist_nth_data(self->scmds, 0);
    res = scommand_to_string(scom);
    unsigned int n = (unsigned int)g_slist_length(self->scmds);   
    for (unsigned int i = 1u; i < n; ++i){
        res = strmerge(res," | ");
        scom = (scommand) g_slist_nth_data(self->scmds,i);
        aux = scommand_to_string(scom);
        res = strmerge(res, aux);
        free(aux);
    }
	return res;
}


