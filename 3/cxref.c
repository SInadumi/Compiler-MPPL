#include "cxref.h"
#include "parser.h"
#include "token-list.h"
/* Memorize symbol table */

// public

// private
static struct LINE *t_lineroot;
static struct TYPE *t_type;
static struct NAMES{
    char *formal_name;
    struct NAMES *nextname;
} *t_namesroot;

static void init_formaltab();
static void release_formaltab();

/* Initialize glocalidroot */
void init_global_idtab(){
    globalidroot = NULL;
    init_formaltab();
}

/* Initialize localidroot */
void init_local_idtab(){
    localidroot = NULL;
    init_formaltab();
}

static void init_formaltab(){
    t_lineroot = NULL;
    t_type = NULL;
    t_namesroot = NULL;
}



/* 
    search the name pointed by np 
    found :     return -> pointer to ID structure
    not found : return -> NULL
*/

struct ID *search_idtab(char *name, int is_global){

    struct ID *p;

    // search among localidroot
    if(!is_global){
        for(p = localidroot; p != NULL; p = p->nextp){
            if(strcmp(name, p->name) == 0) return p;
        }
    }

    // search among globalidroot
    for(p = globalidroot; p != NULL; p = p->nextp){
        if(strcmp(name, p->name) == 0) return p;
    }

    return (NULL);
}

// NAMES構造体に名前を仮保存
int memorize_name(char *name){

    struct NAMES *p;
    char *cname;

    if((p = (struct NAMES *)malloc(sizeof(struct NAMES))) == NULL){
        return error("cannot malloc-1 in memorize name");
    }
    if((cname = (char *)malloc(strlen(name) + 1)) == NULL){
        return error("cannot malloc-2 in memorize name");
    }

    strcpy(cname, name);
    p->formal_name = cname;
    p->nextname = t_namesroot;
    t_namesroot = p;

    return NORMAL;
}

int memorize_type(int ttype, int tsize, struct TYPE *tetp, struct TYPE *tparatp){
    struct TYPE *temp_etp;
    struct TYPE *temp_paratp;
    
    if((t_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
        return error("cannot malloc-1 in memorize type");
    }

    if(tetp != NULL){
        if((temp_etp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
            return error("cannot malloc-2 in memorize type");
        }
        temp_etp = tetp;
    }else{
        temp_etp = NULL;
    }

    if(tparatp != NULL){
        if((temp_paratp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
            return error("cannot malloc-3 in memorize type");
        }
        temp_paratp = tparatp;
    }else{
        temp_paratp = NULL;
    }

    t_type->ttype = ttype;
    t_type->arraysize = tsize;
    t_type->etp = temp_etp;
    t_type->paratp = temp_paratp;

    return NORMAL;

}

// LINE構造体に名前が出現する行を保存
int memorize_linenum(int line){
    struct LINE *p;
    if((p = (struct LINE *)malloc(sizeof(struct LINE))) == NULL){
        return error("cannot malloc-1 in memorize line");
    }
    p->reflinenum = line;
    p->nextep = t_lineroot;
    t_lineroot = p;
    return NORMAL;
}

// ID構造体に保存(is_global -> 1 : globalid, is_global -> 0 : localid)
int define_identifer(char *tpname, int is_formal, int is_global){
    struct ID *p;
    char *temp_name;
    char *temp_procname;
    struct NAMES *name_p, *name_q;
    struct TYPE *t_itp;
    struct LINE *t_line;

    // register loop
    for(name_p = t_namesroot; name_p != NULL; name_p = name_q){
        // Check whether Duplicative name or not
        if((p = search_idtab(name_p->formal_name, is_global)) != NULL){
            return error("NAME is already Defined");
        }

        // malloc pointer
        if((p = (struct ID *)malloc(sizeof(struct ID))) == NULL){
            return error("cannot malloc-1 in define globalidroot");
        }
        if((temp_name = (char *)malloc(strlen(name_p->formal_name) + 1)) == NULL){
            return error("cannot malloc-2 in define globalidroot");
        }
        if(tpname != NULL){
            if((temp_procname = (char *)malloc(strlen(tpname) + 1)) == NULL){
                return error("cannot malloc-3 in define globalidroot");
            }
        }else{
            temp_procname = NULL;
        }
        if((t_itp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
            return error("cannot malloc-4 in define globalidroot");
        }
        
        // assign temp pointers
        strcpy(temp_name, name_p->formal_name);
        if(tpname != NULL){
            strcpy(temp_procname, tpname);
        }else{
            temp_procname = NULL;
        }
        *t_itp = *t_type;

        // register id
        p->name = temp_name;
        p->procname = temp_procname;
        p->itp = t_itp;
        p->ispara = is_formal;
        if(t_lineroot != NULL){
            p->deflinenum = t_lineroot->reflinenum;
            t_line = t_lineroot;
            t_lineroot = t_line;
        }
        p->irefp = NULL;
        p->nextp = globalidroot;

        if(is_global) globalidroot = p;
        else localidroot = p;

        name_q = name_p->nextname;
    }
    
    release_formaltab();
    return NORMAL;
}


// 辞書式順序へ整形
// void Refacter_to_lexicographical(){

// }

void print_idtab(){

}

void release_global_idtab(){
    struct ID *p, *q;
    for(p = globalidroot; p != NULL; p = q){
        free(p->name);
        free(p->procname);
        free(p->itp);
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
        free(p->irefp);
        q = p->nextp;
        free(p);
    }
    init_local_idtab();
}

static void release_formaltab(){
    struct NAMES *p,*q;
    struct LINE *i,*j;

    for(i = t_lineroot; i != NULL; i = j){
        j = i->nextep;
        free(i);
    }

    if(t_type->etp != NULL) free(t_type->etp);
    if(t_type->paratp != NULL) free(t_type->paratp);

    for(p = t_namesroot; p != NULL; p = q){
        free(p->formal_name);
        q = p->nextname;
        free(p);
    }
    init_formaltab();
}

struct TYPE *get_etp_type_structure(){
    // if t_type is NULL, calling func must stop program
    if(t_type != NULL) return t_type;
    else return NULL;
}