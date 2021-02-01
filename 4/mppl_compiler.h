
#ifndef MPPL_H_
#define MPPL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MAXSTRSIZE 1024
#define MAXNUMSIZE 32767
#define MAXLABELSIZE 6

#define ERROR -1
#define NORMAL 0

#define FORMAL_PARAM 1
#define NOT_FORMAL_PARAM 0

#define GLOBAL_PARAM 1
#define LOCAL_PARAM 0

#define TPINT 101
#define TPCHAR 102
#define TPBOOL 103
#define TPARRAY 104
// #define TPARRAYINT 105
// #define TPARRAYCHAR 106
// #define TPARRAYBOOL 107
#define TPPROC 108
#define NUMOFTYPE 8

/* Token */
#define	TNAME		1	/* Name : Alphabet { Alphabet | Digit } */
#define	TPROGRAM	2	/* program : Keyword */
#define	TVAR		3	/* var : Keyword */
#define	TARRAY		4	/* array : Keyword */
#define	TOF			5	/* of : Keyword */
#define	TBEGIN		6	/* begin : Keyword */
#define	TEND		7  	/* end : Keyword */
#define	TIF			8  	/* if : Keyword */
#define	TTHEN		9	/* then : Keyword */
#define	TELSE		10	/* else : Keyword */
#define	TPROCEDURE	11	/* procedure : Keyword */
#define	TRETURN		12	/* return : Keyword */
#define	TCALL		13	/* call : Keyword */
#define	TWHILE		14	/* while : Keyword */
#define	TDO			15 	/* do : Keyword */
#define	TNOT		16	/* not : Keyword */
#define	TOR			17	/* or : Keyword */
#define	TDIV		18 	/* div : Keyword */
#define	TAND		19 	/* and : Keyword */
#define	TCHAR		20	/* char : Keyword */
#define	TINTEGER	21	/* integer : Keyword */
#define	TBOOLEAN	22 	/* boolean : Keyword */
#define	TREADLN		23	/* readln : Keyword */
#define	TWRITELN	24	/* writeln : Keyword */
#define	TTRUE		25	/* true : Keyword */
#define	TFALSE		26	/* false : Keyword */
#define	TNUMBER		27	/* unsigned integer */
#define	TSTRING		28	/* String */
#define	TPLUS		29	/* + : symbol */
#define	TMINUS		30 	/* - : symbol */
#define	TSTAR		31 	/* * : symbol */
#define	TEQUAL		32 	/* = : symbol */
#define	TNOTEQ		33 	/* <> : symbol */
#define	TLE			34 	/* < : symbol */
#define	TLEEQ		35 	/* <= : symbol */
#define	TGR			36	/* > : symbol */
#define	TGREQ		37	/* >= : symbol */
#define	TLPAREN		38 	/* ( : symbol */
#define	TRPAREN		39 	/* ) : symbol */
#define	TLSQPAREN	40	/* [ : symbol */
#define	TRSQPAREN	41 	/* ] : symbol */
#define	TASSIGN		42	/* := : symbol */
#define	TDOT		43 	/* . : symbol */
#define	TCOMMA		44	/* , : symbol */
#define	TCOLON		45	/* : : symbol */
#define	TSEMI		46	/* ; : symbol */
#define	TREAD		47	/* read : Keyword */
#define	TWRITE		48	/* write : Keyword */
#define	TBREAK		49	/* break : Keyword */
#define KEYWORDSIZE	28

/* Instruction set */
#define EOVF 201
#define EOVF1 202
#define E0DIV 203
#define E0DIV1 204
#define EROV  205
#define EROV1 206
#define WRITECHAR 207
#define WRITESTR 208
#define BOVFCHECK 209
#define BOVFLEVEL 210
#define WRITEINT 211
#define MMINT 212
#define WRITEBOOL 213
#define WBTRUE 214
#define WBFALSE 215
#define WRITELINE 216
#define FLUSH 217
#define READCHAR 218
#define READINT 219
#define READLINE 220
#define ONE 221
#define SIX 222
#define TEN 223
#define SPACE 224
#define MINUS 225
#define TAB 226
#define ZERO 227
#define NINE 228
#define NEWLINE 229
#define INTBUF 230
#define OBUFSIZE 231
#define IBUFSIZE 232
#define INP 233
#define OBUF 234
#define IBUF 235
#define RPBBUF 236

struct KEYWORDS{
	char *keyword;
	int keytoken;
};
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
};

struct PARAM{
    struct ID *now;
    struct PARAM *nextp;
};

/* main.c */
extern FILE *input, *output;
extern int error(const char *mes, ...);

/* scan.c */
extern int num_attr;
extern char string_attr[MAXSTRSIZE];
extern int init_scan(char *filename, FILE **input);
extern int scan();
extern int get_linenum(void);
extern void end_scan(FILE *input);

/* parser.c */
#define NUMOFTOKEN	49
extern char *tokenstr[NUMOFTOKEN+1]; /* string of each token */
extern int Parse_program();
extern int Parse_block(char *st_label);
extern int Parse_variable_declaration();
extern int Parse_variable_names();
extern int Parse_variable_name();
extern int Parse_type();
extern int Parse_standard_type();
extern int Parse_array_type();
extern int Parse_subprogram_declaration();
extern int Parse_procedule_name();
extern int Parse_formal_parameters();
extern int Parse_compound_statement();
extern int Parse_statement();
extern int Parse_condition_statement();
extern int Parse_iteration_statement();
extern int Parse_exit_statement();
extern int Parse_call_statement();
extern int Parse_expressions(struct TYPE *fparams);
extern int Parse_return_statement();
extern int Parse_assignment_statement();
extern int Parse_left_part();
extern int Parse_variable();
extern int Parse_expression();
extern int Parse_simple_expression();
extern int Parse_term();
extern int Parse_factor();
extern int Parse_constant();
extern int Parse_multiplicative_operator();
extern int Parse_additive_operator();
extern int Parse_relational_operator();
extern int Parse_input_statement();
extern int Parse_output_statement();
extern int Parse_output_format();
extern int Parse_empty_statement();

/* cxref.c */
extern struct ID *globalidroot;
extern struct ID *localidroot;
extern struct PARAM *param;
extern char *type_str[NUMOFTYPE + 1];
extern void init_global_idtab(void);
extern void init_local_idtab(void);
extern struct ID *search_global_idtab(char *name);
extern struct ID *search_local_idtab(char *name, char *pname);
extern int memorize_name(char *name);
extern int memorize_type(int ttype, int tsize, struct TYPE *tetp, struct PARAM *tparatp);
extern int memorize_linenum(int line);
extern int memorize_proc(char *name, int line);
extern int define_identifer(int is_formal, int is_global);
extern int reference_identifer(char *name, char *pname, int linenum, int refnum, int is_global);
extern struct ID *refactor_to_lexicographical(struct ID *to, struct ID *from);
extern int print_cxref_table(void);
extern void release_global_idtab(void);
extern void relocate_local_idtab(void);
extern struct TYPE *get_etp_type_structure(void);
extern struct PARAM *get_paratp(char *pname);
extern char *get_prev_procname(void);
extern int get_prev_procline(void);

/* mppl_compiler.c */
extern int labelnum;
extern int init_outfile(char *filename, FILE **out);
extern void end_outfile(FILE *out);
extern int create_label(char **label);
extern int print_id_label(struct ID *p);
extern void print_strlabel();
extern int register_strlabel(char *label, char *str);
extern int inst_start(char *program_name, char **st_label);
extern void inst_procedule_params(struct PARAM *params);
extern int inst_write_string();
extern int inst_write_value(int type, int nums);
extern void inst_read(int type);
extern void inst_assignment(int is_subproc, struct ID *ref_id);
extern int inst_variable(struct ID *p, int point_to_array);
extern int inst_expressions(int opr, int is_opr);
extern int inst_expression(int opr);
extern void inst_minus();
extern void inst_simple_expression(int opr);
extern void inst_term(int opr);
extern int inst_factor_char(int type, int exp_type);
extern void INSTRUCTIONS();
#endif