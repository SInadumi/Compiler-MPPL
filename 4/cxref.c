#include "mppl_compiler.h"
/* Memorize symbol table */

/* public */

char *type_str[NUMOFTYPE + 1] = {
    "", "integer", "char", "boolean", "array", "integer", "char", "boolean", "procedule"
};

/* private */
static struct LINE *t_lineroot;
static struct TYPE *t_type ,*paratp_root = NULL;
static struct NAMES{
    char *formal_name;
    struct NAMES *nextname;
} *t_namesroot;
static struct PROC{
    char *prev_procname;
    int prev_proc_line;
} *t_proc = NULL;

static void init_formaltab();
static void release_formaltab();
int compare_names(const void *p, const void *q);
static void print_space_to_stdout(int num);

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
    paratp_root = NULL;
}

/* 
    search the name pointed by np 
    found :     return -> pointer to ID structure
    not found : return -> NULL
*/
struct ID *search_global_idtab(char *name){
    struct ID *p;
    for(p = globalidroot; p != NULL; p = p->nextp){
        if(strcmp(name, p->name) == 0 && p->procname == NULL) return p;
    }
    return NULL;
}
struct ID *search_local_idtab(char *name, char *pname){
    struct ID *p;
    for(p = localidroot; p != NULL; p = p->nextp){
        if(strcmp(name, p->name) == 0 && strcmp(pname, p->procname) == 0) return p;
    }
    return NULL;
}

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

int memorize_proc(char *pname, int line){
    struct PROC *temp_proc;
    char *tpname;
    if((temp_proc = (struct PROC *)malloc(sizeof(struct PROC))) == NULL){
        return error("cannot malloc-1 in memorize procname");
    }
    if(pname != NULL){
        if((tpname = (char *)malloc(strlen(pname) + 1)) == NULL){
            return error("cannot malloc-2 in memorize procname");
        }
        strcpy(tpname, pname);
    }else{
        tpname = NULL;
        //free(temp_proc);
    }
    temp_proc->prev_procname = tpname;
    temp_proc->prev_proc_line = line;
    t_proc = temp_proc;

    return NORMAL;  
}

// ID構造体に保存(is_global -> 1 : globalid, is_global -> 0 : localid)
int define_identifer(int is_formal, int is_global){
    struct ID *p;
    char *temp_name;
    char *temp_procname;
    struct NAMES *name_p, *name_q;
    struct TYPE *t_itp;
    struct LINE *t_line, *t_irefp;

    // register loop
    for(name_p = t_namesroot; name_p != NULL; name_p = name_q){
        // Check whether Duplicative name or not
        if(is_global == LOCAL_PARAM){
            if((p = search_local_idtab(name_p->formal_name, get_prev_procname())) != NULL){
                return error("%s is already Defined", name_p->formal_name);
            }
        }else{
            if((p = search_global_idtab(name_p->formal_name)) != NULL){
                return error("%s is already Defined", name_p->formal_name);
            }
        }

        // malloc pointer
        if((p = (struct ID *)malloc(sizeof(struct ID))) == NULL){
            return error("cannot malloc-1 in define globalidroot");
        }
        if((temp_name = (char *)malloc(strlen(name_p->formal_name) + 1)) == NULL){
            return error("cannot malloc-2 in define globalidroot");
        }
        if(is_global == LOCAL_PARAM){
            if((temp_procname = (char *)malloc(strlen(t_proc->prev_procname) + 1)) == NULL){
                return error("cannot malloc-3 in define globalidroot");
            }
        }else{
            temp_procname = NULL;
        }
        if((t_itp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
            return error("cannot malloc-4 in define globalidroot");
        }
        if((t_irefp = (struct LINE *)malloc(sizeof(struct LINE))) == NULL){
            return error("cannot malloc-5 in define globalidroot");
        }
        t_irefp = NULL;
        
        // assign temp pointers
        strcpy(temp_name, name_p->formal_name);
        if(is_global == LOCAL_PARAM){
            strcpy(temp_procname, t_proc->prev_procname);
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
        p->irefp = t_irefp;

        if(is_global){
            globalidroot = refacter_to_lexicographical(globalidroot, p);
        }else{
            localidroot = refacter_to_lexicographical(localidroot, p);
        }

        name_q = name_p->nextname;
    }
    
    release_formaltab();
    return NORMAL;
}

int reference_identifer(char *name, char *pname, int linenum, int refnum, int is_global){
    /* refnum is pointer to element if name is array */
    struct ID *p;
    struct LINE *t_irefp, *last;
    if(is_global == LOCAL_PARAM){
        if((p = search_local_idtab(name, pname)) == NULL){ 
            if((p = search_global_idtab(name)) == NULL){
                return error("%s(procedule : %s) is not found", name, pname);
            }
        }
    }else{
        if((p = search_global_idtab(name)) == NULL) return error("%s is not found", name);
    }

    if(p->itp->ttype == TPARRAY && p->itp->arraysize <= refnum) return error("Overflow array index");

    if((t_irefp = (struct LINE *)malloc(sizeof(struct LINE))) == NULL){
        return error("cannnot malloc in reference identifer");
    }

    t_irefp->reflinenum = linenum;
    t_irefp->nextep = p->irefp;
    p->irefp = t_irefp;

    return NORMAL;
}

// 辞書式順序へ整形
int compare_names(const void *p, const void *q){
    // p->name > q->name then plus num
    // p->name < q->name then minus num 
    return (strcmp(((struct ID *)p)->name, ((struct ID *)q)->name));
}
struct ID *refacter_to_lexicographical(struct ID *to, struct ID *from){
    struct ID *temp = NULL, *head, *p, *q;

    if(to == NULL){
        to = from;
        return to;
    }
    
    /* Select Head pointer */
    if(compare_names(to, from) > 0) head = from,p = to, q = from->nextp;
    else head = to,p = to->nextp, q = from;;
    temp = head;
    
    /* Lexicographic Sorting */
    while(p != NULL && q != NULL){
        
        if(compare_names(p,q) > 0){
            temp->nextp = q;
            q = q->nextp;
        }else{ 
            temp->nextp = p;
            p = p->nextp;
        }
        temp = temp->nextp;
    }

    if(p == NULL) temp->nextp = q;
    else temp->nextp = p;
    return head;
}

/* print to stdout from globalidroot */
int print_cxref_table(){
    struct ID *p;
    struct TYPE *q;
    struct LINE * r;
    int buf_size = 0;
    int space = 0;
    char buf[MAXSTRSIZE];
    fprintf(stdout, "--------------------------------------------------------------------------\n");
    fprintf(stdout, "Name");
    print_space_to_stdout(20);
    fprintf(stdout, "Type");
    print_space_to_stdout(23);
    fprintf(stdout, "Def. Ref.");
    print_space_to_stdout(14);
    fprintf(stdout, "\n");
    for(p = globalidroot; p != NULL; p = p->nextp){
        // print name:procname
        if(p->procname != NULL){
            buf_size = strlen(p->name) + strlen(p->procname);
            if(buf_size >= MAXSTRSIZE) return error("Overflow to print stdout");
            sprintf(buf, "%s:%s", p->name, p->procname);
        }else{
            buf_size = strlen(p->name);
            if(buf_size >= MAXSTRSIZE) return error("Overflow to print stdout");
            sprintf(buf, "%s", p->name);
        }
        space = 24 - strlen(buf);
        fprintf(stdout, "%s", buf);
        print_space_to_stdout(space);

        // print type
        space = 27;
        buf_size = strlen(type_str[p->itp->ttype - 100]);
        if(buf_size >= MAXSTRSIZE) return error("Overflow to print stdout");
        
        if(p->itp->ttype == TPPROC){
            if(p->itp->paratp != NULL){
                sprintf(buf, "%s(", type_str[p->itp->ttype - 100]);
                for(q = p->itp->paratp; q != NULL; q = q->paratp){
                    buf_size += strlen(type_str[q->ttype - 100]);
                    if(buf_size >= MAXSTRSIZE) return error("Overflow to print stdout");
                    if(q->paratp != NULL) sprintf(buf, "%s%s,", buf, type_str[q->ttype - 100]);
                    else sprintf(buf, "%s%s", buf, type_str[q->ttype - 100]);
                    buf_size = strlen(buf);
                }
                sprintf(buf, "%s)", buf);
            }else{
                sprintf(buf, "%s", type_str[p->itp->ttype - 100]);
            }

        }else if(p->itp->ttype == TPARRAY){
            sprintf(buf, "%s[", type_str[p->itp->ttype - 100]);
            // buf_size += strlen(type_str[p->itp->etp->ttype - 100]) + 1;
            for(q = p->itp->etp; q != NULL; q = q->etp){
                buf_size += strlen(type_str[q->ttype - 100]) + 1;
                if(buf_size >= MAXSTRSIZE) return error("Overflow to print stdout");
                if(q->etp != NULL) sprintf(buf, "%s%s,", buf, type_str[q->ttype - 100]);
                else sprintf(buf, "%s%s", buf, type_str[q->ttype - 100]);
                buf_size = strlen(buf);
            }
            sprintf(buf, "%s]", buf);
        }else{
            sprintf(buf, "%s", type_str[p->itp->ttype - 100]);
        }
        buf_size = strlen(buf);
        space -= buf_size;
        fprintf(stdout, "%s", buf);
        print_space_to_stdout(space);

        // print def, ref
        space = 22;
        buf_size = 2;
        sprintf(buf, "%d|", p->deflinenum);
        for(r = p->irefp; r != NULL; r = r->nextep){
            buf_size = buf_size + 2;
            if(r->nextep != NULL) sprintf(buf, "%s%d,", buf, r->reflinenum);
            else sprintf(buf, "%s%d", buf, r->reflinenum);
            buf_size = strlen(buf);
        }
        buf_size = strlen(buf);
        space -= buf_size;
        fprintf(stdout, "%s", buf);
        print_space_to_stdout(space);
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "--------------------------------------------------------------------------\n");
    return NORMAL;
}

void release_global_idtab(){
    struct ID *p, *q;
    struct TYPE *i, *j;
    struct LINE *a, *b;
    for(p = globalidroot; p != NULL; p = q){
        free(p->name);
        free(p->procname);
        // free (paratp in procedule entity)
        for(i = p->itp; i != NULL; i = j){
            j = i->paratp;
            free(i);
        }
        // free (iretp in ref line entity)
        for(a = p->irefp; a != NULL; a = b){
            b = a->nextep;
            free(a);
        }
        q = p->nextp;
        free(p);
    }
    
    free(t_proc->prev_procname);
    free(t_proc);
    t_proc = NULL;
    init_global_idtab();
}

void relocate_local_idtab(){
    struct ID *temp = localidroot;
    
    globalidroot = refacter_to_lexicographical(globalidroot, localidroot);

    if(paratp_root != NULL){
        if(paratp_root->etp != NULL) free(paratp_root->etp);
        if(paratp_root->paratp != NULL) free(paratp_root->paratp);
        free(paratp_root);
    }

    t_proc->prev_procname = NULL;
    t_proc->prev_proc_line = -1;

    init_local_idtab();
}

static void release_formaltab(){
    struct NAMES *p,*q;
    struct LINE *i,*j;

    for(i = t_lineroot; i != NULL; i = j){
        j = i->nextep;
        free(i);
    }

    //if(t_type->etp != NULL) free(t_type->etp);
    //if(paratp_root == NULL && t_type->paratp != NULL) free(t_type->paratp);
    free(t_type);

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
struct TYPE *get_paratp(char *pname){
    struct TYPE *tparatp;
    struct ID *p,*q;
    for(p = localidroot; p != NULL; p = q){
        if(p->procname == NULL) continue; 
        if(strcmp(p->procname, pname) == 0 && p->ispara == FORMAL_PARAM){
            if((tparatp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL){
                fprintf(stdout, "cannot malloc in get paratp");
                return NULL;
            }
            tparatp->ttype = p->itp->ttype;
            tparatp->arraysize = 0;
            tparatp->etp = NULL;
            tparatp->paratp = paratp_root;
            paratp_root = tparatp;
        }
        q = p->nextp;
    }
    return paratp_root;
}
char *get_prev_procname(){
    return t_proc->prev_procname;
}
int get_prev_procline(){
    return t_proc->prev_proc_line;
}
static void print_space_to_stdout(int num){
    int index;
    for(index = 0; index < num; index++){
        fprintf(stdout, " ");
    }
}