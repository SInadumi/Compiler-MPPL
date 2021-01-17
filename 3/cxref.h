/* cxref.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define FORMAL_PARAM 1
#define NOT_FORMAL_PARAM 0

#define GLOBAL_PARAM 1
#define LOCAL_PARAM 0

#define NUMOFTYPE 8

#define TPINT 101
#define TPCHAR 102
#define TPBOOL 103
#define TPARRAY 104
// #define TPARRAYINT 105
// #define TPARRAYCHAR 106
// #define TPARRAYBOOL 107
#define TPPROC 108

extern char *type_str[NUMOFTYPE + 1];

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
extern struct ID *search_global_idtab(char *name);
extern struct ID *search_local_idtab(char *name, char *pname);
extern int memorize_name(char *name);
extern int memorize_type(int ttype, int tsize, struct TYPE *tetp, struct TYPE *tparatp);
extern int memorize_linenum(int line);
extern int memorize_proc(char *name, int line);
extern int define_identifer(int is_formal, int is_global);
extern int reference_identifer(char *name, char *pname, int linenum, int refnum, int is_global);
extern struct ID *refacter_to_lexicographical(struct ID *to, struct ID *from);
extern int print_cxref_table(void);
extern void release_global_idtab(void);
extern void relocate_local_idtab(void);
extern struct TYPE *get_etp_type_structure(void);
extern struct TYPE *get_paratp(char *pname);
extern char *get_prev_procname(void);
extern int get_prev_procline(void);