#include "cxref.h"
/* Memorize symbol table */

// public

// private

/* Initialize glocalidroot */
void init_global_idtab(){
    globalidroot = NULL;
}

/* Initialize localidroot */
void init_local_idtab(){
    localidroot = NULL;
}

/* search the name pointed by np */
struct ID *search_idtab(char *np){

    return (NULL);
}

void memorize_type(){

}

void print_idtab(){

}

void release_global_idtab(){
    struct ID *p, *q;
    for(p = globalidroot; p != NULL; p = q){
        free(p->name);
        free(p->procname);
        free(p->itp);
        free(p->ispara);
        free(p->deflinenum);
        free(p->irefp);
        q = p->nextp;
        free(p);
    }
    init_global_idtab();
}

void release_local_idtab(){
    struct ID *p, *q;
    for(p = localidroot; p != NULL; p = q){
        free(p->name);
        free(p->procname);
        free(p->itp);
        free(p->ispara);
        free(p->deflinenum);
        free(p->irefp);
        q = p->nextp;
        free(p);
    }
    init_local_idtab();
}
