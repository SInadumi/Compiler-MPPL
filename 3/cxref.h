/* cxref.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define FORMAL_PARAM 1
#define NOT_FORMAL_PARAM 0
#define GLOBAL_PARAM 1
#define LOCAL_PARAM 0

/*
    TYPE struct has infomation regarding type
    ttype     : TPINT, TPCHAR, TPBOOL, TPARRAY, TPARRRAYINT, TPARRAYCHAR,
                TPARRAYBOOL, TPPROC
    arraysize : size of array, if TPARRAY
    etp       : pointer to element, if ttyep is TPARRAY
    paratp    : pointer to parameter's type list, if ttype is TPPROC
*/
struct TYPE{
    int ttype;
    int arraysize;
    struct TYPE *etp;
    struct TYPE *paratp; 
};

/*
    LINE struct has infomation regarding line
    reflinenum : number of line
    nextep     : pointer to element
*/
struct LINE{
    int reflinenum;
    struct LINE *nextep;
};

/* 
    ID struct has infomation regarding id
    name
    procname    : if local type, procedule name within which this name is defined
                  if glocal type, NULL
    itp
    ispara      : if formal parameter, ispara is 1
                  else , ispara is 0
    deflinenum
    irefp
    nexttp
*/

struct ID{
    char *name;
    char *procname;
    struct TYPE *itp;
    int ispara;
    int deflinenum;
    struct LINE *irefp;
    struct ID *nextp;
}*globalidroot, *localidroot;

extern void init_global_idtab(void);
extern void init_local_idtab(void);
extern struct ID *search_idtab(char *name, int is_global);
extern int memorize_name(char *name);
extern int memorize_type(int ttype, int tsize, struct TYPE *tetp, struct TYPE *tparatp);
extern int memorize_linenum(int line);
extern int memorize_procname(char *name);
extern int define_identifer(int is_formal, int is_global);
extern void print_cxref_table(void);
extern void release_global_idtab(void);
extern void release_local_idtab(void);
extern void release_procname(void);
extern struct TYPE *get_etp_type_structure();