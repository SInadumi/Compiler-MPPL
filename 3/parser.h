/* parser.h */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "token-list.h"
#include "cxref.h"

#define ERROR -1
#define NORMAL 0

/* pretty_printer.c */

/* string of each token */
#define NUMOFTOKEN	49
extern char *tokenstr[NUMOFTOKEN+1];


extern int Parse_program(FILE *fp);
extern int Parse_block(FILE *fp);
extern int Parse_variable_declaration(FILE *fp);
extern int Parse_variable_names(FILE *fp);
extern int Parse_variable_name(FILE *fp);
extern int Parse_type(FILE *fp);
extern int Parse_standard_type(FILE *fp);
extern int Parse_array_type(FILE *fp);
extern int Parse_subprogram_declaration(FILE *fp);
extern int Parse_procedule_name(FILE *fp);
extern int Parse_formal_parameters(FILE *fp);
extern int Parse_compound_statement(FILE *fp);
extern int Parse_statement(FILE *fp);
extern int Parse_condition_statement(FILE *fp);
extern int Parse_iteration_statement(FILE *fp);
extern int Parse_exit_statement(FILE *fp);
extern int Parse_call_statement(FILE *fp);
extern int Parse_expressions(FILE *fp, struct TYPE *fparams);
extern int Parse_return_statement(FILE *fp);
extern int Parse_assignment_statement(FILE *fp);
extern int Parse_left_part(FILE *fp);
extern int Parse_variable(FILE *fp);
extern int Parse_expression(FILE *fp);
extern int Parse_simple_expression(FILE *fp);
extern int Parse_term(FILE *fp);
extern int Parse_factor(FILE *fp);
extern int Parse_constant(FILE *fp);
extern int Parse_multiplicative_operator(FILE *fp);
extern int Parse_additive_operator(FILE *fp);
extern int Parse_relational_operator(FILE *fp);
extern int Parse_input_statement(FILE *fp);
extern int Parse_output_statement(FILE *fp);
extern int Parse_output_format(FILE *fp);
extern int Parse_empty_statement();
extern void Generate_steps_of_mpl();