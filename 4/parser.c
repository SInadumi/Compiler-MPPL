#include "mppl_compiler.h"

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
static int is_empty = 0;    // Does Exist empty statement
static int is_iterate = 0;  // Judge whether this is scanning of iteration statement or not
static int token = 0;       // Token code Buffer
static int is_global = GLOBAL_PARAM;
static char *label_exit;

static int Check_Standard_Type(int TYPE);

int Parse_program(){
    char *st_label = NULL;

    /* Initialize global id table*/
    init_global_idtab();

    token = scan();
    if(token != TPROGRAM) return error("'program' is not found");
    token = scan();

    if(token != TNAME) return error("expect program name behind 'program'");
    if(memorize_proc(NULL, get_linenum()) == ERROR) return ERROR;
    if(inst_start(string_attr, &st_label) == ERROR) return ERROR;
    token = scan();
    
    if(token != TSEMI) return error("Semicolon is not found in program statement");
    token = scan();

    /* Parse(block) */
    if(Parse_block(st_label) == ERROR) return ERROR;
 
    if(token != TDOT) return error("Colon is not found in program statement");
    token = scan();

    /* print cxref table */
    if(print_cxref_table() == ERROR) return ERROR;

    /* print str Label and inst */
    print_strlabel();
    INSTRUCTIONS();
    /* write 'END' in output */
    fprintf(output, "\tEND\n");

    /* release global id table */
    release_global_idtab();

    return NORMAL; 
}

int Parse_block(char *st_label){
    /* repeat variable declaration or subproblem declaration */
    while(token == TVAR || token == TPROCEDURE){

        if(token == TVAR){ 
            is_global = GLOBAL_PARAM;
            if(Parse_variable_declaration() == ERROR) return ERROR;
        }else if(token == TPROCEDURE){
            is_global = LOCAL_PARAM;
            if(Parse_subprogram_declaration() == ERROR) return ERROR;
        }else return error("'var' or 'procedure' is not found");
        
    }
    fprintf(output, "%s\n", st_label);

    /* check compound statement */
    is_global = GLOBAL_PARAM;
    if(Parse_compound_statement() == ERROR) return ERROR;

    /* write 'RET' in output */
    fprintf(output, "\tRET\n");

    return NORMAL; 
}

int Parse_variable_declaration(){

    if(token != TVAR) return error("'var' is not found");
    token = scan();
    
    /* Parse(variable names) */
    if(Parse_variable_names() == ERROR) return ERROR;

    if(token != TCOLON) return error("Colon is not found in variable declaration statement");
    token = scan();

    /* Parse(type) */
    if(Parse_type() == ERROR) return ERROR;

    // register ID(globalidroot)
    if(define_identifer(NOT_FORMAL_PARAM, is_global) == ERROR) return ERROR;


    if (token != TSEMI) return error("Semicolon is not found in variable declaration statement");
    token = scan();

    while(token == TNAME){

        /* Parse(variable names) */
        if(Parse_variable_names() == ERROR) return ERROR;

        if(token != TCOLON) return error("Colon is not found in variable declaration statement");
        token = scan();

        /* Parse(type) */
        if(Parse_type() == ERROR) return ERROR;

        /* register ID(glbalidroot) */
        if(define_identifer(NOT_FORMAL_PARAM, is_global) == ERROR) return ERROR;

        if (token != TSEMI) return error("Semicolon is not found in variable declaration statement");
        token = scan();

    }

    return NORMAL;
}
int Parse_variable_names(){

    /* Parse(variable name) */
    if(Parse_variable_name() == ERROR) return ERROR;

    while(token == TCOMMA){
        token = scan();
        if(Parse_variable_name() == ERROR) return ERROR;
    }

    return NORMAL;
}

int Parse_variable_name(){

    if(token != TNAME) error("expect [NAME]. For the purpose of using valiable token");
    
    if(memorize_name(string_attr) == ERROR) return ERROR;
    if(memorize_linenum(get_linenum()) == ERROR) return ERROR;

    token = scan();
    
    return NORMAL;
}

int Parse_type(){
    int TYPE = ERROR;
    if(token == TARRAY){
        if((TYPE = Parse_array_type()) == ERROR) return ERROR;
    }else if(token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        if((TYPE = Parse_standard_type()) == ERROR) return ERROR;
    }else return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'");
    return TYPE;
}

int Parse_standard_type(){
    int TYPE = ERROR;
    if(token == TINTEGER){
        TYPE = TPINT;
    }else if(token == TBOOLEAN){
        TYPE = TPBOOL;
    }else if(token == TCHAR){
        TYPE = TPCHAR;
    }else{
        return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" ); 
    }

    if(memorize_type(TYPE, 0, NULL, NULL) == ERROR) return ERROR;
    token = scan();
    return TYPE;
}

int Parse_array_type(){

    int TYPE = TPARRAY;
    struct TYPE *etp_TYPE = NULL;
    int SIZEofarray = 0;           // size of array type

    if(token != TARRAY) return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" );
    token = scan();

    if(token != TLSQPAREN) return error("Square brackets is not found in [array type] statement");
    token = scan();

    if(token != TNUMBER) return error("expect [NUMBER] in [array type] statement");
    if((SIZEofarray = num_attr) == 0) return error("INDEX of array must be more one character ");
    token = scan();

    if(token != TRSQPAREN) return error("Square brackets is not found in [array type] statement");
    token = scan();

    if(token != TOF) return error("'of' is not found in [array type] statement");
    token = scan();

    if(Parse_standard_type(input) == ERROR) return ERROR;
    if((etp_TYPE = get_etp_type_structure()) == NULL) return error("array entity is not found in [array type] statement");

    if(etp_TYPE->ttype == TPINT) etp_TYPE->ttype = TPINT;
    else if(etp_TYPE->ttype == TPCHAR) etp_TYPE->ttype = TPCHAR;
    else if(etp_TYPE->ttype == TPBOOL) etp_TYPE->ttype = TPBOOL;

    if(memorize_type(TYPE, SIZEofarray, etp_TYPE, NULL) == ERROR) return ERROR;
    return TYPE;
}

int Parse_subprogram_declaration(){

    struct PARAM *param = NULL;

    /* initialize localidtab */
    init_local_idtab();
    
    if(token != TPROCEDURE) return error("'procedule' is not found");
    token = scan();

    /* Parse(procedule name) */
    if(Parse_procedule_name() == ERROR) return ERROR;

    /* Parse(formal paramenters) */
    if(token == TLPAREN && (Parse_formal_parameters() == ERROR)) return ERROR;

    if (token != TSEMI) return error("Semicolon is not found in procedule statement");
    token = scan();
    param = get_paratp(get_prev_procname());
    if(token == TVAR){
        if(Parse_variable_declaration()) return ERROR;
    }

    memorize_name(get_prev_procname());
    memorize_linenum(get_prev_procline());
    memorize_type(TPPROC, 0, NULL, param);
    define_identifer(NOT_FORMAL_PARAM, GLOBAL_PARAM);

    /* print process arguments in output */
    inst_procedule_params(param);

    /* Parse(compound statement) */
    if(Parse_compound_statement() == ERROR) return ERROR;    

    if (token != TSEMI) return error("Semicolon is not found in procedule statement");
    token = scan();

    /* release local idtab */
    relocate_local_idtab();

    /* write 'RET' in output */
    fprintf(output, "\tRET\n");

    return NORMAL;
}

int Parse_procedule_name(){
    if(token != TNAME) error("expect [NAME] in procedule statement");
    if(memorize_proc((char *)string_attr, get_linenum()) == ERROR) return ERROR;

    token = scan();
    return NORMAL;
}

int Parse_formal_parameters(){

    int TYPE = ERROR;

    if(token != TLPAREN) return error("Parentheses is not found in procedule statement");
    token = scan();

    if(Parse_variable_names() == ERROR) return ERROR;

    if(token != TCOLON) return error("Colon is not found in procedule statement");
    token = scan();

    if((TYPE = Parse_type()) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE) == ERROR) return error("type is expected integer or boolean or char in formal parameter");
    
    /* register ID(localidroot) */
    if(define_identifer(FORMAL_PARAM, is_global) == ERROR) return ERROR;

    while(token == TSEMI){
        token = scan();

        if(Parse_variable_names() == ERROR) return ERROR;

        if(token != TCOLON) return error("Colon is not found in procedule statement");
        token = scan();

        if((TYPE = Parse_type()) == ERROR) return ERROR;
        if(Check_Standard_Type(TYPE) == ERROR) return error("type is expected integer or boolean or char in formal parameter");
        
        /* register ID(localidroot) */
        if(define_identifer(FORMAL_PARAM, is_global) == ERROR) return ERROR;

    }

    if(token != TRPAREN) return error("parentheses is not found in procedule statement");
    token = scan();

    return NORMAL;

}

int Parse_compound_statement(){

    if(token != TBEGIN) return error("'begin' is not found in compound statement");
    token = scan();

    /* Parse(statement) */
    if (Parse_statement() == ERROR) return ERROR;
    
    while(token == TSEMI){
        token = scan();
        if(Parse_statement() == ERROR) return ERROR;
    }

    if(token != TEND) return error("'end' is not found.");
    if(is_empty == 1) is_empty = 0;
    token = scan();

    return NORMAL;
}

int Parse_statement(){

    switch(token){
        case TNAME:
            if(Parse_assignment_statement() == ERROR) return ERROR;
            break;

        case TIF:
            if(Parse_condition_statement() == ERROR) return ERROR;
            break;

        case TWHILE:
            if(Parse_iteration_statement() == ERROR) return ERROR;
            break;

        case TBREAK:
            if(Parse_exit_statement() == ERROR) return ERROR;
            break;

        case TCALL:
            if(Parse_call_statement() == ERROR) return ERROR;
            break;

        case TRETURN:
            if(Parse_return_statement() == ERROR) return ERROR;
            break;

        case TREAD:
        case TREADLN:
            if(Parse_input_statement() == ERROR) return ERROR;
            break;

        case TWRITE:
        case TWRITELN:
            if(Parse_output_statement() == ERROR) return ERROR;
            break;

        case TBEGIN:
            if(Parse_compound_statement() == ERROR) return ERROR;
            break;

        default:
            return Parse_empty_statement();
            break;
    }
    return NORMAL;
}

int Parse_condition_statement(){

    int TYPE = ERROR;
    char *label_if, *label_else;

    if(create_label(&label_if) == ERROR) return ERROR;

    if(token != TIF) return error("'if' is not found");
    token = scan();

    if((TYPE = Parse_expression()) == ERROR) return ERROR;
    if(TYPE != TPBOOL) return error("type of expression is expected boolean in condition statement");

    /* print if command in output */
    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJZE\t%s\n", label_if);

    if(token != TTHEN) return error("'then' is not found");
    token = scan();

    if(Parse_statement() == ERROR) return ERROR;
    
    if(token == TELSE){
        
        if(create_label(&label_else) == ERROR) return ERROR;
        fprintf(output, "\tJUMP\t%s\n", label_else);
        fprintf(output, "%s\n", label_if);

        token = scan();

        if(Parse_statement() == ERROR) return ERROR;

        fprintf(output, "%s\n", label_else);
    }else{
        fprintf(output, "%s\n", label_if);
    }
    return NORMAL;
}

int Parse_iteration_statement(){

    int TYPE = ERROR;
    char *label = NULL, *holder_label = NULL, *t_label_exit = NULL;

    if(token != TWHILE) return error("'while' is not found");
    token = scan();

    if(create_label(&label) == ERROR) return ERROR;
    if(create_label(&t_label_exit) == ERROR) return ERROR;
    holder_label = label_exit;
    fprintf(output, "%s\n", label);

    if((TYPE = Parse_expression()) == ERROR) return ERROR;
    if(TYPE != TPBOOL) return error("type of expression is expected boolean in iteration statement");

    if(token != TDO) return error("'do' is not found");
    token = scan();

    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJZE\t%s\n", t_label_exit);
    label_exit = t_label_exit;

    is_iterate++;
    if(Parse_statement() == ERROR) return ERROR;
    is_iterate--;
    
    label_exit = holder_label;
    fprintf(output, "\tJUMP\t%s\n", label);
    fprintf(output, "%s\n", t_label_exit);

    return NORMAL;
}

int Parse_exit_statement(){
    if(token != TBREAK) return error("'break' is not found");
    if(is_iterate > 0){
        token = scan();
    }else{
        return error("'break' is not exist in iteration statement"); 
    }
    fprintf(output, "\tJUMP\t%s\n", label_exit);
    return NORMAL;
}

int Parse_call_statement(){

    struct ID *fparams;
    int line;

    if(token != TCALL) return error("'call' is not found");
    token = scan();

    line = get_linenum();
    if(Parse_procedule_name() == ERROR) return ERROR;
    
    if((fparams = search_global_idtab(get_prev_procname())) == NULL){
        return error("%s is not found", get_prev_procname());
    }
    if(is_global == LOCAL_PARAM && strcmp(get_prev_procname(), fparams->name) == 0){
        return error("recursive proc statement is not permission");
    }
    if((reference_identifer(get_prev_procname(), get_prev_procname(), line, 0, is_global)) == ERROR) return ERROR;

    if(token == TLPAREN){
        token = scan();

        if(Parse_expressions(fparams->itp->paratp) == ERROR) return ERROR;
        if(token != TRPAREN) return error("parentheses is not found in call statement");
        token = scan();
    }
    fprintf(output, "\tCALL\t$%s\n", get_prev_procname());
    return NORMAL;
}

int Parse_expressions(struct TYPE *fparams){

    int TYPE = ERROR;
    struct TYPE *tfparams = fparams;

    if((TYPE = Parse_expression()) == ERROR) return ERROR;
    if(tfparams == NULL) return error("number of formal and expression is not matched");
    if(TYPE != tfparams->ttype) return error("type of formal and expression is not matched");

    
    while(token == TCOMMA){
        token = scan();
        
        tfparams = tfparams->paratp;

        if((TYPE = Parse_expression()) == ERROR) return ERROR;
        if(tfparams == NULL) return error("number of formal and expression is not matched");
        if(TYPE != tfparams->ttype) return error("type of formal and expression is not matched");
    }

    return NORMAL;
}

int Parse_return_statement(){
    if(token != TRETURN) return error("'return' is not found");
    token = scan();
    
    /* write 'RET' in output */
    fprintf(output, "\tRET\n");

    return NORMAL;
}

int Parse_assignment_statement(){
    
    int TYPE_left_part = ERROR;
    int TYPE_expression = ERROR;

    if((TYPE_left_part = Parse_left_part()) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE_left_part) == ERROR) return error("Type of left part is expected integer or boolean or char");

    if(token != TASSIGN) return error("':=' is not found in assign statement");
    token = scan();

    if((TYPE_expression = Parse_expression()) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE_expression) == ERROR) return error("Type of expression is expected integer or boolean or char in assignment statement");
    
    if(TYPE_left_part != TYPE_expression) return error("Type of LEFT_PART and EXPRESSION should be equal");

    // スタックトップ->式の値，一段下に左辺値
    fprintf(output, "\tPOP\tgr2\n");
    fprintf(output, "ST\tgr1,0,gr2\n");

    return NORMAL;
}

int Parse_left_part(){
    int TYPE = ERROR;
    if((TYPE = Parse_variable()) == ERROR) return ERROR;

    return TYPE;
}

int Parse_variable(){

    int TYPE = ERROR;
    int TYPE_statement = ERROR;
    int point_to_array = -1;
    struct ID *p;
    if(is_global == LOCAL_PARAM){
        if((p = search_local_idtab((char *)string_attr, get_prev_procname())) == NULL){
            if((p = search_global_idtab((char *)string_attr)) == NULL){
                return error("%s is not found", string_attr);
            }
        }
    }else{
        if((p = search_global_idtab((char *)string_attr)) == NULL) return error("%s is not found", string_attr);
    }
    TYPE = p->itp->ttype;

    if(Parse_variable_name() == ERROR) return ERROR;

    if(token == TLSQPAREN){
        if(TYPE != TPARRAY) return error("type of expression is expected array in variable statement");
        if(p->itp->etp == NULL) return error("array entity is not found");
        TYPE = p->itp->etp->ttype;

        token = scan();

        if(token == TNUMBER) point_to_array = num_attr;

        if((TYPE_statement = Parse_expression()) == ERROR) return ERROR;
        if(TYPE_statement != TPINT) return error("type of expression should be integer in variable statement");

        if(token != TRSQPAREN) return error("Square brackets is not found in variable statement");
        token = scan();
    }

    if(reference_identifer(p->name, get_prev_procname(), get_linenum(), point_to_array, is_global) == ERROR) return ERROR;
    /* print load or load adress in variable */
    if(inst_variable(p, point_to_array) == ERROR) return ERROR;
    return TYPE;
}

int Parse_expression(){

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;
    int opr = ERROR;

    if((TYPE = Parse_simple_expression()) == ERROR) return ERROR;

    TYPE_operator = TYPE;

    while(token == TEQUAL || token == TNOTEQ || token == TLE ||
        token == TLEEQ || token == TGR || token == TGREQ ){
            
            opr = token;
            fprintf(output, "\tPUSH\t0,gr1\n");

            if((TYPE = Parse_relational_operator()) == ERROR) return ERROR;
            if((TYPE_operand = Parse_simple_expression()) == ERROR) return ERROR;
            //if(TYPE != TYPE_operand) return error("type of operator and relational operator should be equal");
            if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
            TYPE_operator = TYPE_operand;
            inst_expression(opr);
    }

    return TYPE;
}

int Parse_simple_expression(){

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;
    
    int is_plus_or_minus = 0;
    int is_minus = 0;
    int opr = ERROR;

    if(token == TPLUS){
        is_plus_or_minus = 1;
        token = scan();
    }else if(token == TMINUS){
        is_plus_or_minus = 1;
        is_minus = 1;
        token = scan();
    }
    
    if((TYPE = Parse_term()) == ERROR) return ERROR;
    if(is_plus_or_minus == 1 && TYPE != TINTEGER) return error("type of term should be TINTEGER in simple expression");
    TYPE_operator = TYPE;

    if(is_minus == 1) inst_minus();

    while(token == TPLUS || token == TMINUS || token == TOR){
        
        opr = token;
        fprintf(output, "\tPUSH\t0,gr1\n");

        if((TYPE = Parse_additive_operator()) == ERROR) return ERROR;
        if((TYPE_operand = Parse_term()) == ERROR) return ERROR;

        inst_simple_expression(opr);

        //if(TYPE != TYPE_operand) return error("type of operator and additive operator should be equal");
        if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
        TYPE_operator = TYPE_operand;

    }

    return TYPE;
}

int Parse_term(){

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;

    int opr = ERROR;

    if((TYPE = Parse_factor()) == ERROR) return ERROR;

    TYPE_operator = TYPE;

    while(token == TSTAR || token == TDIV || token == TAND){

        opr = token;
        fprintf(output, "\tPUSH\t0,gr1\n");

        if((TYPE = Parse_multiplicative_operator()) == ERROR) return ERROR;
        if((TYPE_operand = Parse_factor()) == ERROR) return ERROR;

        inst_term(opr);

        //if(TYPE != TYPE_operand) return error("type of operator and multiplicative operator should be equal");
        if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
        TYPE_operator = TYPE_operand;
    }

    return TYPE;

}

int Parse_factor(){

    int TYPE_expression = ERROR;

    int TYPE = ERROR;
    switch(token){
        case TNAME:
            if((TYPE = Parse_variable()) == ERROR) return ERROR;
            break;

        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            if((TYPE = Parse_constant()) == ERROR) return ERROR;
            break;

        case TLPAREN:
            token = scan();

            if((TYPE = Parse_expression()) == ERROR) return ERROR;
            if(token != TRPAREN) return error("parentheses is not found in factor");

            token = scan();
            break;

        case TNOT:
            token = scan();
            if((TYPE = Parse_factor()) == ERROR) return ERROR;
            if(TYPE != TPBOOL) return error("expect boolean type after [not] statement");
            break;

        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if((TYPE = Parse_standard_type()) == ERROR) return ERROR;
            if(token != TLPAREN) return error("left parentheses is not found in factor");
            token = scan();
            
            if((TYPE_expression = Parse_expression()) == ERROR) return ERROR;
            if(token != TRPAREN) return error("right parentheses is not found in factor");

            if(Check_Standard_Type(TYPE_expression) == ERROR) return error("type of expression is expected integer or boolean or char in factor statement");
            
            token = scan();
            break;

        default:
            return error("factor statement was not read");
            break;
    }

    return TYPE;
}

int Parse_constant(){

    int TYPE = ERROR;

    if(token == TNUMBER){
        fprintf(output, "\tLAD\tgr1,%d\n", num_attr);
        TYPE = TPINT;
    }else if(token == TFALSE){
        fprintf(output, "\tLAD\tgr1,0\n");
        TYPE = TPBOOL;
    }else if(token == TTRUE){
        fprintf(output, "\tLAD\tgr1,1\n");
        TYPE = TPBOOL;
    }else if(token == TSTRING){
        if(strlen(string_attr) == 1){
            fprintf(output, "\tLAD\tgr1,%d\n", string_attr[0]);
            TYPE = TPCHAR;
        }else{
            return error("expect only one length in STRING element in constant variable");
        }
    }else{
        return error("expect [NUMBER] or 'false' or 'true' or [STRING]");
    }

    token = scan();

    return TYPE;
}

int Parse_multiplicative_operator(){

    int TYPE = ERROR;

    if(token == TSTAR || token == TDIV) TYPE = TPINT;
    else if(token == TAND) TYPE = TPBOOL;
    else return error("expect '*' or 'div' or 'and' in multiple operator");

    token = scan();
    return TYPE;
}

int Parse_additive_operator(){

    int TYPE = ERROR;

    if(token == TPLUS || token == TMINUS) TYPE = TPINT;
    else if(token == TOR) TYPE = TPBOOL;
    else return error("expect '+' or '-' or 'or' in additive operator");

    token = scan();
    return TYPE;
}

int Parse_relational_operator(){

    if(!( token == TEQUAL || token == TNOTEQ || token == TLE   ||
          token == TLEEQ  || token == TGR    || token == TGREQ )){
            return error("expect relational operator");
    }
    token = scan();

    return TPBOOL;
}
int Parse_input_statement(){

    int TYPE = ERROR;
    int is_readln = 0;

    if(token == TREADLN) is_readln = 1;
    if(token == TREAD || token == TREADLN){
        token = scan(); 
    }
    else return error("'read' or 'readln' is not found");

    if(token == TLPAREN){
        token = scan(); 

        if((TYPE = Parse_variable()) == ERROR) return ERROR;
        if(TYPE != TPINT && TYPE != TPCHAR){ 
            return error("valiable statement is expected type of char or integer in input statement");
        }
        /* print inst read */
        inst_read(TYPE);

        while(token == TCOMMA){
            token = scan(); 
            if((TYPE = Parse_variable()) == ERROR) return ERROR;
            if(TYPE != TPINT && TYPE != TPCHAR){ 
                return error("type of valiable statement is expected char or integer in input statement");
            }
        }

        if(token != TRPAREN) return error("expect parentheses in input statement");
        token = scan();    
    }
    if(is_readln) fprintf(output, "\tCALL\tREADLINE\n");
    return NORMAL;
}

int Parse_output_statement(){
    int is_writeln = 0;
    if(token == TWRITELN) is_writeln = 1;
    if(token == TWRITE || token == TWRITELN){
        token = scan(); 
    }
    else return error("'write' or 'writeln' is not found");

    if(token == TLPAREN){
        token = scan();

        if(Parse_output_format() == ERROR) return ERROR;

        while(token == TCOMMA){
            token = scan(); 

            if(Parse_output_format() == ERROR) return ERROR;
        }

        if(token != TRPAREN) return error("expect parentheses in output statement");
        token = scan();    
    }
    if(is_writeln) fprintf(output, "\tCALL\tWRITELINE\n");
    return NORMAL;
}

int Parse_output_format(){

    int TYPE = ERROR;

    if(token == TSTRING && strlen(string_attr) > 1){
        inst_write_string();
        token = scan();
        return NORMAL;
    }

    switch(token){
        case TSTRING:
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
            if((TYPE = Parse_expression()) == ERROR) return ERROR;
            if(Check_Standard_Type(TYPE) == ERROR) return error("type of expression is expected integer or boolean or char in output statement");

            if(token == TCOLON){
                token = scan();
        
                if(token != TNUMBER) return error("expect [NUMBER] in output statement");
                inst_write_value(TYPE, num_attr);
                token = scan();
            }else{
                inst_write_value(TYPE, 0);
            }
            break;
        default:
            return error("Output format is not found");
            break;
    }
    return NORMAL;
}
int Parse_empty_statement(){
    is_empty = 1;
    return NORMAL;
}

/*
    Check Type  
    Case : type is belong to standard  ->   return NORMAL
    Case : type isn't belong to standard -> return ERROR
*/
static int Check_Standard_Type(int TYPE){
    
    if(TYPE == TPINT || TYPE == TPBOOL || TYPE == TPCHAR ) return NORMAL;
    else return ERROR;

}