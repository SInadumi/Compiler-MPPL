#include "token-list.h"
#include "pretty_printer.h"

/* public */
char *tokenstr[NUMOFTOKEN+1] = {
	"",
	"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
	 "else", "procedure", "return", "call", "while", "do", "not", "or", 
	"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
	 "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", 
	">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};

/* private */
static int is_empty = 0;
static int token;

int Parse_program(FILE *fp){

    token = scan(fp);
    if(token != TPROGRAM) return error("'program' is not found.");
    fprintf(stdout,"%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TNAME) return error("expect [NAME] behind 'program'");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    
    if(token != TSEMI) return error("[;] is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    /* Parse(block) */
    if(Parse_block(fp)) return ERROR;
 
    if(token != TDOT) return error("[.] is not found.");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    return NORMAL; 
}

int Parse_block(FILE *fp){

    /* repeat variable declaration or subproblem declaration */
    while(token == TVAR || token == TPROCEDURE){
 
        if(token == TVAR){ 
            if(Parse_variable_declaration(fp)) return ERROR;
        }else if(token == TPROCEDURE){
             if(Parse_subprogram_declaration(fp)) return ERROR;
        }else return error("'var' or 'procedure' is not found");
        
    }
    /* check compound statement */
    if(Parse_compound_statement(fp)) return ERROR;

    return NORMAL; 
}

/*
memo: 'var'はtokenに先読みされた状態で関数に入る
*/
int Parse_variable_declaration(FILE *fp){

    if(token != TVAR) return error("'var' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    
    /* Parse(variable names) */
    if(Parse_variable_names(fp)) return ERROR;

    if(token != TCOLON) return error("[:] is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    /* Parse(type) */
    if(Parse_type(fp)) return ERROR;

    if (token != TSEMI) return error("[;] is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);
  
    while(token == TNAME){
        
        /* Parse(variable names) */
        if(Parse_variable_names(fp)) return ERROR;

        if(token != TCOLON) return error("[:] is not found");
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        /* Parse(type) */
        if(Parse_type(fp)) return ERROR;

        if (token != TSEMI) return error("[;] is not found");
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);
    }

    return NORMAL;
}
int Parse_variable_names(FILE *fp){
    
    /* Parse(variable name) */
    if(Parse_variable_name(fp)) return ERROR;

    while(token == TCOMMA){
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan(fp);
        if(Parse_variable_name(fp)) return ERROR;
    }

    return NORMAL;
}

int Parse_variable_name(FILE *fp){

    if(token != TNAME) error("expect [NAME]");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    return NORMAL;
}

int Parse_type(FILE *fp){
    if(token == TARRAY){
        if(Parse_array_type(fp)) return ERROR;
    }else if(token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        if(Parse_standard_type(fp)) return ERROR;
    }else return error("expect 'array' or 'integer' or 'boolean' or 'char' in type");
    return NORMAL;
}

int Parse_standard_type(FILE *fp){
    switch(token){
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;
        default:
            return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" ); 
            break;
    }
    return NORMAL;
}
int Parse_array_type(FILE *fp){
    if(token != TARRAY) return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" );
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TLSQPAREN) return error("Square brackets is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TNUMBER) return error("expect [NUMBER] in [array type] statement");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);

    if(token != TRSQPAREN) return error("Square brackets is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TOF) return error("'of' is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_standard_type(fp)) return ERROR;

    return NORMAL;
}

/*
memo: 'procedule'はtokenに先読みされた状態で関数に入る
*/
int Parse_subprogram_declaration(FILE *fp){
    if(token != TPROCEDURE) return error("'procedule' is not found");
    
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    /* Parse(procedule name) */
    if(Parse_procedule_name(fp)) return ERROR;

    /* Parse(formal paramenters) */
    if(token == TLPAREN && Parse_formal_parameters(fp)) return ERROR;

    if (token != TSEMI) return error("[;] is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    if(token == TVAR && Parse_variable_declaration(fp)) return ERROR;

    /* Parse(compound statement) */
    if(Parse_compound_statement(fp)) return ERROR;    

    if (token != TSEMI) return error("[;] is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    return NORMAL;
}

int Parse_procedule_name(FILE *fp){
    if(token != TNAME) error("expect [NAME] in procedule statement");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    return NORMAL;
}

int Parse_formal_parameters(FILE *fp){

    if(token != TLPAREN) return error("parentheses is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_variable_names(fp)) return ERROR;

    if(token != TCOLON) return error("colon is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_type(fp)) return ERROR;


    while(token == TSEMI){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if(Parse_variable_names(fp)) return ERROR;

        if(token != TCOLON) return error("colon is not found in procedule statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if(Parse_type(fp)) return ERROR;

    }

    if(token != TRPAREN) return error("parentheses is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    return NORMAL;

}
int Parse_compound_statement(FILE *fp){
    if(token != TBEGIN) return error("'begin' is not found.");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    /* Parse(statement) */
    if (Parse_statement(fp)) return ERROR;
    
    while(token == TSEMI){
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);
        if(Parse_statement(fp)) return ERROR;
    }

    if(token != TEND) return error("'end' is not found.");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    return NORMAL;
}
int Parse_statement(FILE *fp){

    switch(token){
        case TNAME:
            if(Parse_assignment_statement(fp)) return ERROR;
            break;

        case TIF:
            if(Parse_condition_statement(fp)) return ERROR;
            break;

        case TWHILE:
            if(Parse_iteration_statement(fp)) return ERROR;
            break;

        case TBREAK:
            if(Parse_exit_statement(fp)) return ERROR;
            break;

        case TCALL:
            if(Parse_call_statement(fp)) return ERROR;
            break;

        case TRETURN:
            if(Parse_return_statement(fp)) return ERROR;
            break;

        case TREAD:
        case TREADLN:
            if(Parse_input_statement(fp)) return ERROR;
            break;

        case TWRITE:
        case TWRITELN:
            if(Parse_output_statement(fp)) return ERROR;
            break;

        case TBEGIN:
            if(Parse_compound_statement(fp)) return ERROR;
            break;

        default:
            return Parse_empty_statement(fp);
            break;
    }
    return NORMAL;
}

int Parse_condition_statement(FILE *fp){
    if(token != TIF) return error("'if' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_expression(fp)) return ERROR;

    if(token != TTHEN) return error("'then' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    if(Parse_statement(fp)) return ERROR;
    
    if(token == TELSE){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
        if(Parse_statement(fp)) return ERROR;
    }    
    return NORMAL;
}

int Parse_iteration_statement(FILE *fp){
    if(token != TWHILE) return error("'while' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_expression(fp)) return ERROR;

    if(token != TDO) return error("'do' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    if(Parse_statement(fp)) return ERROR;
    return NORMAL;
}

int Parse_exit_statement(FILE *fp){
    if(token != TBREAK) return error("'break' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);
    return NORMAL;
}

int Parse_call_statement(FILE *fp){
    if(token != TCALL) return error("'call' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    if(Parse_procedule_name(fp)) return ERROR;

    if(token == TLPAREN){
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        if(Parse_expressions(fp)) return ERROR;
        if(token != TRPAREN) return error("parentheses is not found in call statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
    }
    return NORMAL;
}
int Parse_expressions(FILE *fp){
    if(Parse_expression(fp)) return ERROR;
    
    while(token == TCOMMA){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if(Parse_expression(fp)) return ERROR;
    }

    return NORMAL;
}

int Parse_return_statement(FILE *fp){
    if(token != TRETURN) return error("'return' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    return NORMAL;
}

int Parse_assignment_statement(FILE *fp){
    
    if(Parse_left_part(fp)) return ERROR;

    if(token != TASSIGN) return error("':=' is not found in assign statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_expression(fp)) return ERROR;

    return NORMAL;
}

int Parse_left_part(FILE *fp){
    if(Parse_variable(fp)) return ERROR;
    return NORMAL;
}

int Parse_variable(FILE *fp){
    if(Parse_variable_name(fp)) return ERROR;

    if(token == TLSQPAREN){
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        if(Parse_statement(fp)) return ERROR;

        if(token != TRSQPAREN) return error("Square brackets is not found in variable statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
    }
    
    return NORMAL;
}
int Parse_expression(FILE *fp){
    if(Parse_simple_expression(fp)) return ERROR;

    while(token == TEQUAL || token == TNOTEQ || token == TLE ||
        token == TLEEQ || token == TGR || token == TGREQ ){
            if(Parse_relational_operator(fp)) return ERROR;
            if(Parse_simple_expression(fp)) return ERROR;
    }
    return NORMAL;
}

int Parse_simple_expression(FILE *fp){

    if(token == TPLUS || token == TMINUS){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
    }
    
    if(Parse_term(fp)) return ERROR;

    while(token == TPLUS || token == TMINUS || token == TOR){
        if(Parse_additive_operator(fp)) return ERROR;
        if(Parse_term(fp)) return ERROR;
    }

    return NORMAL;
}

int Parse_term(FILE *fp){

    if(Parse_factor(fp)) return ERROR;

    while(token == TSTAR || token == TDIV || token == TAND){
        if(Parse_multiplicative_operator(fp)) return ERROR;
        if(Parse_factor(fp)) return ERROR;
    }

    return NORMAL;
}

int Parse_factor(FILE *fp){
    switch(token){
        case TNAME:
            if(Parse_variable(fp)) return ERROR;
            break;
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            if(Parse_constant(fp)) return ERROR;
            break;
        case TLPAREN:
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);

            if(Parse_expression(fp)) return ERROR;
            if(token != TRPAREN) return error("parentheses is not found in factor");
            
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;
        case TNOT:
            if(Parse_factor(fp)) return ERROR;
            break;
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if(Parse_standard_type(fp)) return ERROR;
            if(token != TLPAREN) return error("left parentheses is not found in factor");
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            if(Parse_expression(fp)) return ERROR;
            if(token != TRPAREN) return error("right parentheses is not found in factor");
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;
        default:
            return error("factor statement wan not read");
            break;
    }
    return NORMAL;
}

int Parse_constant(FILE *fp){
    if(!(token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING)){
        return error("expect [NUMBER] or 'false' or 'true' or [STRING]");
    }
    if(token == TSTRING || token == TNUMBER) fprintf(stdout, "%s ", string_attr);
    else fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return NORMAL;
}
int Parse_multiplicative_operator(FILE *fp){
    if(!(token == TSTAR || token == TDIV || token == TAND)){
        return error("expect '*' or 'div' or 'and'");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return NORMAL;
}
int Parse_additive_operator(FILE *fp){
    if(!(token == TPLUS || token == TMINUS || token == TOR)){
        return error("expect '+' or '-' or 'or'");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return NORMAL;
}
int Parse_relational_operator(FILE *fp){
    if(!( token == TEQUAL || token == TNOTEQ || token == TLE   ||
          token == TLEEQ  || token == TGR    || token == TGREQ     )){
            return error("expect relational operator");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return NORMAL;
}
int Parse_input_statement(FILE *fp){
    if(token == TREAD || token == TREADLN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 
    }
    else return error("'read' or 'readln' is not found");

    if(token == TLPAREN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 

        if(Parse_variable(fp)) return ERROR;

        while(token == TCOMMA){
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp); 
            if(Parse_variable(fp)) return ERROR;
        }

        if(token != TRPAREN) return error("expect parentheses in input statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);    
    }
    return NORMAL;
}

int Parse_output_statement(FILE *fp){

    if(token == TWRITE || token == TWRITELN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 
    }
    else return error("'write' or 'writeln' is not found");

    if(token == TLPAREN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 
        if(Parse_output_format(fp)) return ERROR;

        while(token == TCOMMA){
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp); 
            if(Parse_output_format(fp)) return ERROR;
        }

        if(token != TRPAREN) return error("expect parentheses in input statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);    
    }
    return NORMAL;
}
int Parse_output_format(FILE *fp){
    
    switch(token){
        case TSTRING:
            if(string_length > 1){
                fprintf(stdout, "%s ", string_attr);
                token = scan(fp);
            }
            break;
        case TPLUS:
        case TMINUS:
        case TNAME:
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TLPAREN:
        case TNOT:
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if(Parse_expression(fp)) return ERROR;
            if(token == TCOLON){
                fprintf(stdout, "%s ", tokenstr[token]);
                token = scan(fp);
        
                if(token != TNUMBER) return error("expect [NUMBER] in output statement");
                fprintf(stdout, "%s ", string_attr);
                token = scan(fp);
            }
            break;
        default:
            return error("Output format is not found");
            break;
    }
    return NORMAL;
}
int Parse_empty_statement(FILE *fp){
    is_empty = 1;
    return NORMAL;
}