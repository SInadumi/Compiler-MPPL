#include "token-list.h"
#include "parser.h"
#include "cxref.h"

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
static int now_step = 0;    // Ref program steps

static int Check_Standard_Type(int TYPE);

int Parse_program(FILE *fp){

    /* Initialize global id table*/
    init_global_idtab();

    token = scan(fp);
    if(token != TPROGRAM) return error("'program' is not found");
    fprintf(stdout,"%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TNAME) return error("expect [NAME] behind 'program'");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    
    if(token != TSEMI) return error("Semicolon is not found in program statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    /* Parse(block) */
    if(Parse_block(fp)) return ERROR;
 
    if(token != TDOT) return error("Colon is not found in program statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    /* release global id table */
    release_global_idtab();

    return NORMAL; 
}

int Parse_block(FILE *fp){
// すべての名前は，プログラムテキスト中で使用される前に宣言されていなければならない
    /* repeat variable declaration or subproblem declaration */
    while(token == TVAR || token == TPROCEDURE){

        // この変数宣言部で宣言されている変数名のスコープはプログラム全体である
        if(token == TVAR){ 
            if(Parse_variable_declaration(fp)) return ERROR;
        }else if(token == TPROCEDURE){
            break;
            if(Parse_subprogram_declaration(fp)) return ERROR;
        }else return error("'var' or 'procedure' is not found");
        
    }

    // この複合文中で使われる変数名はこの変数宣言部で宣言されていなければならない
    // この複合文中で使われる手続き名は副プログラム宣言で宣言されていなければならない

    /* check compound statement */
    if(Parse_compound_statement(fp)) return ERROR;

    return NORMAL; 
}

int Parse_variable_declaration(FILE *fp){

    // この変数宣言部で宣言されている変数名のスコープはプログラム全体である
    int TYPE = ERROR;

    now_step++;
    Generate_steps_of_mpl();

    if(token != TVAR) return error("'var' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    
    /* Parse(variable names) */
    if(Parse_variable_names(fp)) return ERROR;

    if(token != TCOLON) return error("Colon is not found in variable declaration statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    /* Parse(type) */
    if((TYPE = Parse_type(fp)) == ERROR) return ERROR;

    // register ID
    if(define_identifer(NULL, NOT_FORMAL_PARAM, GLOBAL_PARAM)) return ERROR;

    if (token != TSEMI) return error("Semicolon is not found in variable declaration statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    while(token == TNAME){
        now_step++;
        Generate_steps_of_mpl();

        /* Parse(variable names) */
        if(Parse_variable_names(fp)) return ERROR;

        if(token != TCOLON) return error("Colon is not found in variable declaration statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        /* Parse(type) */
        if((TYPE = Parse_type(fp)) == ERROR) return ERROR;

        /* register ID */
        if(define_identifer(NULL, NOT_FORMAL_PARAM, GLOBAL_PARAM)) return ERROR;

        if (token != TSEMI) return error("Semicolon is not found in variable declaration statement");
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        now_step--;
    }

    now_step--;
    return NORMAL;
}
int Parse_variable_names(FILE *fp){

    // 同じスコープを持つ同じ名前が複数回宣言されていてはならない

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
    
    if(token != TNAME) error("expect [NAME]. For the purpose of using valiable token");
    if(memorize_name(string_attr)) return ERROR;
    if(memorize_linenum(get_linenum())) return ERROR;

    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    
    return NORMAL;
}

int Parse_type(FILE *fp){
    int TYPE = ERROR;
    if(token == TARRAY){
        if((TYPE = Parse_array_type(fp)) == ERROR) return ERROR;
    }else if(token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        if((TYPE = Parse_standard_type(fp)) == ERROR) return ERROR;
    }else return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'");
    return TYPE;
}

int Parse_standard_type(FILE *fp){
    int TYPE = token;
    switch(token){
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            memorize_type(token, 0, NULL, NULL);
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;
        default:
            return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" ); 
            break;
    }
    return TYPE;
}

int Parse_array_type(FILE *fp){

    // 標準型を要素型とする配列型を表す．"符号なし整数"は配列の大きさ(要素数)を表す
    // 従って，"符号なし整数"の値は1 以上でなくてはならない
    // 添字の下限は0 であり，上限は配列の大きさより1 小さな値である(C と同じ)

    int TYPE = token;
    int SIZEofarray = 0;           // size of array type

    if(token != TARRAY) return error("unknown type. expect 'integer' or 'boolean' or 'char' or 'array'" );
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TLSQPAREN) return error("Square brackets is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TNUMBER) return error("expect [NUMBER] in [array type] statement");
    if((SIZEofarray = strlen(string_attr)) == 0) return error("INDEX of array must be more one character ");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);

    if(token != TRSQPAREN) return error("Square brackets is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(token != TOF) return error("'of' is not found in [array type] statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    // 配列の中身の型
    if(Parse_standard_type(fp) == ERROR) return ERROR;

    return TYPE;
}

int Parse_subprogram_declaration(FILE *fp){
    now_step++;
    Generate_steps_of_mpl();
    
    // この手続き名のスコープはプログラム全体である
    // この手続き名をこの複合文内で使用することはできない．再帰呼び出しはできない
    
    if(token != TPROCEDURE) return error("'procedule' is not found");
    
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    /* Parse(procedule name) */
    if(Parse_procedule_name(fp)) return ERROR;

    // この仮引数部と変数宣言部で宣言されている名前のスコープはこの複合文である．

    /* Parse(formal paramenters) */
    if(token == TLPAREN && Parse_formal_parameters(fp)) return ERROR;

    if (token != TSEMI) return error("Semicolon is not found in procedule statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    if(token == TVAR && Parse_variable_declaration(fp)) return ERROR;

    // すべての名前は，プログラムテキスト中で使用される前に宣言されていなければならない
    // 静的スコープルールを採用する．手続き名，変数名を問わず，同じスコープを持つ複数の同
    // じ名前の宣言があった場合には二重定義エラーである
    
    /* Parse(compound statement) */
    Generate_steps_of_mpl();
    if(Parse_compound_statement(fp)) return ERROR;    

    if (token != TSEMI) return error("Semicolon is not found in procedule statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    now_step--;
    return NORMAL;
}

int Parse_procedule_name(FILE *fp){
    if(token != TNAME) error("expect [NAME] in procedule statement");
    fprintf(stdout, "%s ", string_attr);
    token = scan(fp);
    return NORMAL;
}

int Parse_formal_parameters(FILE *fp){

    // 仮引数部に現れる型は標準型でなくてはならない
    // 変数名の並びにある変数名の型をその次の":"の次の型とする

    int TYPE = ERROR;

    if(token != TLPAREN) return error("Parentheses is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_variable_names(fp)) return ERROR;

    if(token != TCOLON) return error("Colon is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if((TYPE = Parse_type(fp)) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE)) return error("type is expected integer or boolean or char in formal parameter");

    while(token == TSEMI){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if(Parse_variable_names(fp)) return ERROR;

        if(token != TCOLON) return error("Colon is not found in procedule statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if((TYPE = Parse_type(fp)) == ERROR) return ERROR;
        if(Check_Standard_Type(TYPE)) return error("type is expected integer or boolean or char in formal parameter");

    }

    if(token != TRPAREN) return error("parentheses is not found in procedule statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    return NORMAL;

}

int Parse_compound_statement(FILE *fp){

    if(token != TBEGIN) return error("'begin' is not found in compound statement");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    now_step++;
    Generate_steps_of_mpl();

    /* Parse(statement) */
    if (Parse_statement(fp)) return ERROR;
    
    while(token == TSEMI){
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        Generate_steps_of_mpl();
        if(Parse_statement(fp)) return ERROR;
    }

    if(token != TEND) return error("'end' is not found.");
    now_step--;
    if(is_empty == 1){
        is_empty = 0;
        fprintf(stdout, "\r");
    }else fprintf(stdout, "\n");

    Generate_steps_of_mpl();
    fprintf(stdout, "%s ", tokenstr[token]);
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
            return Parse_empty_statement();
            break;
    }
    return NORMAL;
}

int Parse_condition_statement(FILE *fp){

    // 式の型はboolean 型でなくてはならない

    int TYPE = ERROR;

    if(token != TIF) return error("'if' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if((TYPE = Parse_expression(fp)) == ERROR) return ERROR;
    if(TYPE != TBOOLEAN) return error("type of expression is expected boolean in condition statement");

    if(token != TTHEN) return error("'then' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    now_step++;
    Generate_steps_of_mpl();
    if(Parse_statement(fp)) return ERROR;
    now_step--;
    
    if(token == TELSE){
        fprintf(stdout, "\n");
        Generate_steps_of_mpl();
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        now_step++;
        Generate_steps_of_mpl();
        if(Parse_statement(fp)) return ERROR;
        now_step--;
    }    
    return NORMAL;
}

int Parse_iteration_statement(FILE *fp){

    // 式の型はboolean 型でなくてはならない

    if(token != TWHILE) return error("'while' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_expression(fp)) return ERROR;

    if(token != TDO) return error("'do' is not found");
    fprintf(stdout, "%s\n", tokenstr[token]);
    token = scan(fp);

    is_iterate++;
    Generate_steps_of_mpl();
    if(Parse_statement(fp)) return ERROR;
    is_iterate--;
    return NORMAL;
}

int Parse_exit_statement(FILE *fp){
    if(token != TBREAK) return error("'break' is not found");
    if(is_iterate > 0){
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan(fp);
    }else return error("'break' is not exist in iteration statement");        
    return NORMAL;
}

int Parse_call_statement(FILE *fp){

    // 手続き名は副プログラム宣言で手続き名として宣言されていなくてはならない

    if(token != TCALL) return error("'call' is not found");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if(Parse_procedule_name(fp)) return ERROR;

    if(token == TLPAREN){
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan(fp);

        if(Parse_expressions(fp)) return ERROR;
        if(token != TRPAREN) return error("parentheses is not found in call statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
    }
    return NORMAL;
}
int Parse_expressions(FILE *fp){

    // 式の数はその手続き名の宣言の仮引数部の変数の数と一致していなくてはならない
    // 式の並びがない場合や，仮引数部がないという場合はそれぞれの数は0 とする
    // 式と仮引数部の変数は順序で対応し，対応する式と変数は同じ標準型でなくてはならない

    int TYPE = ERROR;

    int num_of_formal_params = 0;   // 仮引数部の変数の数
    

    if((TYPE = Parse_expression(fp)) == ERROR) return ERROR;
    
    while(token == TCOMMA){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);

        if((TYPE = Parse_expression(fp)) == ERROR) return ERROR;
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

    // 左辺部と式の型は同じ型で標準型でなくてはならない
    
    int TYPE_left_part = ERROR;
    int TYPE_expression = ERROR;

    if((TYPE_left_part = Parse_left_part(fp)) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE_left_part)) return error("Type of left part is expected integer or boolean or char");

    if(token != TASSIGN) return error("':=' is not found in assign statement");
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    if((TYPE_expression = Parse_expression(fp)) == ERROR) return ERROR;
    if(Check_Standard_Type(TYPE_expression)) return error("Type of expression is expected integer or boolean or char in assignment statement");
    
    if(TYPE_left_part != TYPE_expression) return error("Type of LEFT_PART and EXPRESSION should be equal");

    return NORMAL;
}

int Parse_left_part(FILE *fp){
    int TYPE = ERROR;
    if(Parse_variable(fp)) return ERROR;

    // 型を探して返す
    fprintf(stdout, "型を探して返す処理を追加する(left part)");
    TYPE = NORMAL;

    return TYPE;
}

int Parse_variable(FILE *fp){

    // 式が付いているときの変数名の型はarray 型，式の型はinteger 型でなくてはならない
    // そのときの変数の型はarray 型の要素型である．
    // 式が付いていない時の変数の型は変数名の型である

    int TYPE = ERROR;
    int TYPE_statement = ERROR;
    if((TYPE = Parse_variable_name(fp)) == ERROR) return ERROR;

    if(token == TLSQPAREN){
        TYPE = TARRAY;
        fprintf(stdout, "%s\n", tokenstr[token]);
        token = scan(fp);

        if((TYPE_statement = Parse_statement(fp)) == ERROR) return ERROR;
        if(TYPE_statement != TINTEGER) return error("type of expression should be integer in variable statement");

        if(token != TRSQPAREN) return error("Square brackets is not found in variable statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
    }
    
    return TYPE;
}
int Parse_expression(FILE *fp){

    // 一つの単純式だけの式の型はその単純式の型である

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;

    if((TYPE = Parse_simple_expression(fp)) == ERROR) return ERROR;
    TYPE_operator = TYPE;

    while(token == TEQUAL || token == TNOTEQ || token == TLE ||
        token == TLEEQ || token == TGR || token == TGREQ ){
            
            if((TYPE = Parse_relational_operator(fp)) == ERROR) return ERROR;
            if((TYPE_operand = Parse_simple_expression(fp)) == ERROR) return ERROR;
            if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
            TYPE_operator = TYPE_operand;
    }

    return TYPE;
}

int Parse_simple_expression(FILE *fp){

    // 一つの項だけの単純式の型はその項の型である
    // "+"があるとき左の項の型はinteger 型でなくてはならない．結果の型はinteger 型である

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;
    
    int check_plus = 0;

    if(token == TPLUS || token == TMINUS){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);
        check_plus = 1;
    }
    
    if((TYPE = Parse_term(fp)) == ERROR) return ERROR;
    if(check_plus == 1 && TYPE != TINTEGER) return error("type of term should be TINTEGER in simple expression");

    TYPE_operator = TYPE;

    while(token == TPLUS || token == TMINUS || token == TOR){

        if((TYPE = Parse_additive_operator(fp)) == ERROR) return ERROR;
        if((TYPE_operand = Parse_term(fp)) == ERROR) return ERROR;
        if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
        TYPE_operator = TYPE_operand;

    }

    return TYPE;
}

int Parse_term(FILE *fp){
    
    // 一つの因子だけの項の型はその因子の型である

    int TYPE = ERROR;
    int TYPE_operator = ERROR;
    int TYPE_operand = ERROR;

    if((TYPE = Parse_factor(fp)) == ERROR) return ERROR;

    TYPE_operator = TYPE;

    while(token == TSTAR || token == TDIV || token == TAND){

        if((TYPE = Parse_multiplicative_operator(fp)) == ERROR) return ERROR;
        if((TYPE_operand = Parse_factor(fp)) == ERROR) return ERROR;
        if(TYPE_operator != TYPE_operand) return error("type of operator and operand should be equal");
        TYPE_operator = TYPE_operand;
    }

    return TYPE;

}

int Parse_factor(FILE *fp){

    // 変数や定数のとき，結果の因子の型はそれぞれの型である
    // "(" 式")"のとき，結果の因子の型は式の型である．
    // "not"の被演算子の型はboolean 型でなくてはならない．結果の因子の型はboolean 型である
    // 標準型"(" 式")"のとき，結果の因子の型はその標準型である．
    // 標準型"(" 式")"の式の型は標準型でなくてはならない

    int TYPE_expression = ERROR;

    int TYPE = ERROR;
    switch(token){
        case TNAME:
            if((TYPE = Parse_variable(fp)) == ERROR) return ERROR;
            break;

        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            if((TYPE = Parse_constant(fp)) == ERROR) return ERROR;
            break;

        case TLPAREN:
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);

            if((TYPE = Parse_expression(fp)) == ERROR) return ERROR;
            if(token != TRPAREN) return error("parentheses is not found in factor");
            
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;

        case TNOT:
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            if((TYPE = Parse_factor(fp)) == ERROR) return ERROR;
            if(TYPE != TBOOLEAN) return error("expect boolean type after [not] statement");
            break;

        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if((TYPE = Parse_standard_type(fp)) == ERROR) return ERROR;
            if(token != TLPAREN) return error("left parentheses is not found in factor");
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            
            if((TYPE_expression = Parse_expression(fp)) == ERROR) return ERROR;
            if(token != TRPAREN) return error("right parentheses is not found in factor");

            if(Check_Standard_Type(TYPE_expression)) return error("type of expression is expected integer or boolean or char in factor statement");
            
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp);
            break;

        default:
            return error("factor statement was not read");
            break;
    }

    return TYPE;
}

int Parse_constant(FILE *fp){
    
    // "符号なし整数"のとき，定数の型はinteger 型，"false"と"true"のときはboolean 型である
    // 文字列は長さが1 でなくてはならず，そのときの定数の型はchar 型である

    int TYPE = ERROR;

    if(token == TNUMBER){
        TYPE = TINTEGER;
    }else if(token == TFALSE || token == TTRUE){
        TYPE = TBOOLEAN;
    }else if(token == TSTRING){
        if(strlen(string_attr) == 1){
            TYPE = TCHAR;
        }else{
            return error("expect only one length in STRING element in constant variable");
        }
    }else{
        return error("expect [NUMBER] or 'false' or 'true' or [STRING]");
    }

    token = scan(fp);

    return TYPE;
}

int Parse_multiplicative_operator(FILE *fp){

    // "*"と"div"の被演算子の型はinteger 型でなくてはならない．結果の型はinteger 型である
    // "and"の被演算子の型はboolean 型でなくてはならない．結果の型はboolean 型である

    int TYPE = ERROR;
    if(token == TSTAR || token == TDIV) TYPE = TINTEGER;
    else if(token == TAND) TYPE = TBOOLEAN;
    else return error("expect '*' or 'div' or 'and' in multiple operator");

    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return TYPE;
}

int Parse_additive_operator(FILE *fp){
    
    // "+"と"-"の被演算子の型はinteger 型でなくてはならない．結果の型はinteger 型である
    // "or"の被演算子の型はboolean 型でなくてはならない．結果の型はboolean 型である

    int TYPE = ERROR;

    if(token == TPLUS || token == TMINUS) TYPE = TINTEGER;
    else if(token == TOR) TYPE = TBOOLEAN;
    else return error("expect '+' or '-' or 'or' in additive operator");

    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);
    return TYPE;
}

int Parse_relational_operator(FILE *fp){

    // 関係演算子の被演算子の型は同じ標準型でなくてはならない．結果の型はboolean 型である

    if(!( token == TEQUAL || token == TNOTEQ || token == TLE   ||
          token == TLEEQ  || token == TGR    || token == TGREQ )){
            return error("expect relational operator");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan(fp);

    return TBOOLEAN;
}
int Parse_input_statement(FILE *fp){

    // 変数の型はinteger 型かchar 型でなくてはならない

    int TYPE = ERROR;

    if(token == TREAD || token == TREADLN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 
    }
    else return error("'read' or 'readln' is not found");

    if(token == TLPAREN){
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp); 

        if((TYPE = Parse_variable(fp)) == ERROR) return ERROR;
        if(TYPE != TINTEGER && TYPE != TCHAR){ 
            return error("valiable statement is expected type of char or integer in input statement");
        }

        while(token == TCOMMA){
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan(fp); 
            if((TYPE = Parse_variable(fp)) == ERROR) return ERROR;
            if(TYPE != TINTEGER && TYPE != TCHAR){ 
                return error("type of valiable statement is expected char or integer in input statement");
            }
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

        if(token != TRPAREN) return error("expect parentheses in output statement");
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan(fp);    
    }
    return NORMAL;
}

int Parse_output_format(FILE *fp){

    // 式の型は標準型でなくてはならない
    // 文字列は長さが0 か2 以上である．

    int TYPE = ERROR;

    switch(token){
        case TSTRING:
            if(strlen(string_attr) == 1){
                return error("length of STRING should be more two or zero");
            }
            else{
                fprintf(stdout, "%s ", string_attr);
                token = scan(fp);
                break;
            }
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
            if((TYPE = Parse_expression(fp)) == ERROR) return ERROR;
            if(Check_Standard_Type(TYPE)) return error("type of expression is expected integer or boolean or char in output statement");

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
int Parse_empty_statement(){
    is_empty = 1;
    return NORMAL;
}

void Generate_steps_of_mpl(){
    int index = 0;
    for(index = 0; index < now_step; index++){
        fprintf(stdout, "\t");
    }
}

/*
    Check Type  
    Case : type is belong to standard  ->   return NORMAL
    Case : type isn't belong to standard -> return ERROR
*/
int Check_Standard_Type(int TYPE){
    
    if(TYPE == TINTEGER || TYPE == TBOOLEAN || TYPE == TCHAR) return NORMAL;
    else return ERROR;

}