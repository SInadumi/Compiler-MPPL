/* cxref.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
extern struct ID *search_idtab(char *np);
extern void memorize_type(void);
extern void print_idtab(void);
extern void release_global_idtab(void);
extern void release_local_idtab(void);