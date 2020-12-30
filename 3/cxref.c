#include "cxref.h"
#include "parser.h"
/* Memorize symbol table */

// public

// private
static struct LINE * t_lineroot;
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
    t_namesroot = NULL;
    t_lineroot = NULL;
}

/* search the name pointed by np */
// struct ID *search_idtab(char *np){

//     return (NULL);
// }

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

// ID構造体に保存(globalid)
int define_globalid(struct TYPE *itp, char *tpname){
    // struct ID *p;
    // char *temp_name;
    // char *temp_procname;
    // struct NAMES *name_p, *name_q;
    // struct TYPE *t_itp;
    // struct LINE *tline;

    // // register loop
    // for(name_p = t_namesroot; name_p != NULL; name_p = name_q){
    //     // Check whether Duplicative name or not
    //     if((p = search_idtab(name_p->formal_name)) != NULL){
    //         return error("NAME is already Defined");
    //     }

    //     // malloc pointer
    //     if((p = (struct ID *)malloc(sizeof(struct ID)) == NULL)){
    //         return error("cannot malloc-1 in define globalidroot");
    //     }
    //     if((temp_name = (char *)malloc(strlen(name_p->formal_name) + 1)) == NULL){
    //         return error("cannot malloc-2 in define globalidroot");
    //     }
    //     if((temp_procname = (char *)malloc(strlen(tpname) + 1)) == NULL){
    //         return error("cannot malloc-3 in define globalidroot");
    //     }
    //     if((t_itp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
    //         return error("cannot malloc-4 in define globalidroot");
    //     }
        
    //     // assign temp pointers
    //     strcpy(temp_name, name_p->formal_name);
    //     if(tpname != NULL){
    //         strcpy(temp_procname, tpname);
    //     }else{
    //         temp_procname = NULL;
    //     }
    //     *t_itp = *itp;

    //     // register name and type
    //     p->name = temp_name;
    //     p->procname = temp_procname;
    //     p->itp = t_itp;

    //     p->nextp = globalidroot;
    //     globalidroot = p;

    //     name_q = name_p->nextname;
    // }
    
    // release_formaltab();
    // return NORMAL;
}

// ID構造体に保存(localid)
// int Define_localid(int type){

//     release_formaltab();
//     return NORMAL;
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
    for(p = t_namesroot; p != NULL; p = q){
        free(p->formal_name);
        q = p->nextname;
        free(p);
    }
    for(i = t_lineroot; i != NULL; i = j){
        j = i->nextep;
        free(i);
    }
    init_formaltab();
}