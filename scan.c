#include "token-list.h"

/* public */
int init_scan(char *filename);
int scan(void);
int get_linenum(void);
void end_scan(void);
int num_attr;
char string_attr[MAXSTRSIZE];
int string_length;

/* private */
static int Check_Keyword(char element);
static int Skip_Digits(char attr_element);
static int Skip_String(char string_element);
static char Skip_Comment(char skip_character);
FILE *fp;
static int row_num = 1;
static char next_buf;  //先読み文字

/* keyword list */
struct KEY key[KEYWORDSIZE] = {
	{"and", 	TAND	},
	{"array",	TARRAY	},
	{"begin",	TBEGIN	},
	{"boolean",	TBOOLEAN},
	{"break",	TBREAK  },
	{"call",	TCALL	},
	{"char",	TCHAR	},
	{"div",		TDIV	},
	{"do",		TDO	},
	{"else",	TELSE	},
	{"end",		TEND	},
	{"false",	TFALSE	},
	{"if",		TIF	},
	{"integer",	TINTEGER},
	{"not",		TNOT	},
	{"of",		TOF	},
	{"or",		TOR	},
	{"procedure", TPROCEDURE},
	{"program",	TPROGRAM},
	{"read",	TREAD	},
	{"readln",	TREADLN },
	{"return", 	TRETURN },
	{"then",	TTHEN	},
	{"true",	TTRUE	},
	{"var",		TVAR	},
	{"while",	TWHILE	},
	{"write",	TWRITE  },
	{"writeln",	TWRITELN}
};

/* 
    Initialize file
    Case : file open error -> return -1
    Case : successed :     -> return 0
 */
int init_scan(char *filename){
    fp = fopen(filename, "r");
    if(fp == NULL) return -1;
    next_buf = fgetc(fp);
    return 0;
}

/*
    Scan one token
    Case : contents is keyword     -> return TokenCode
    Case : content isn't keyword   -> return Name TokenCode
    Case : content is digit string -> return digit string
    Case : EOF or Error            -> return -1
*/
int scan(){
    char prev_buf;  //現在の文字
    int token_code = 0;

    prev_buf = next_buf;

    while(!token_code){

        prev_buf = next_buf;
        next_buf = fgetc(fp);
        if(prev_buf == EOF) return -1;
        
        switch (prev_buf){
        
        // symbol
        case '<':
            token_code = TLE;
            // token_codeの上書き
            if(next_buf == '>' || next_buf == '=') next_buf = fgetc(fp);
            if(next_buf == '>') token_code = TNOTEQ;
            else if(next_buf == '=') token_code = TLEEQ;
            break;

        case '>':
            token_code = TGR;
            // token_codeの上書き
            if(next_buf == '='){
                next_buf = fgetc(fp);
                token_code = TGREQ;
            }
            break;
        case ':':
            token_code = TCOLON;
            // token_codeの上書き
            if(next_buf == '='){
                next_buf = fgetc(fp);
                token_code = TASSIGN;
            }
            break;
        case '+':
            token_code = TPLUS;
            break;
        case '-':
            token_code = TMINUS;
            break;
        case '*': 
            token_code = TSTAR;
            break;
        case '=':
            token_code = TEQUAL;
            break;
        case '(':
            token_code = TLPAREN;
            break;
        case ')':
            token_code = TRPAREN;
            break;
        case '[':
            token_code = TLSQPAREN;
            break;
        case ']':
            token_code = TRSQPAREN;
            break;
        case '.':
            token_code = TDOT;
            break;
        case ',':
            token_code = TCOMMA;
            break;
        case ';':
            token_code = TSEMI;
            break;

        /* separator */
        case ' ':
        case '\t':
            break;
        case '{':
        case '\n':
            row_num++;
            if(next_buf == '\r') next_buf = fgetc(fp);
            break;
        case '\r':
            row_num++;
            if(next_buf == '\n') next_buf = fgetc(fp);
            break;

        /* comment */
        case '/':
            next_buf = Skip_Comment(prev_buf);
            if(next_buf < 0) token_code = -1;
            break;

        // string
        case (char)39:  
            token_code = Skip_String(prev_buf);
            break;

        default:
            /* name or keyword */
            if((int)prev_buf >= (int)'A' && (int)prev_buf <= (int)'z'){
                token_code = Check_Keyword(prev_buf);
            }

            /* digit */
            if((int)prev_buf >= (int)'0' && (int)prev_buf <= (int)'9'){
                token_code = Skip_Digits(prev_buf);
                break;
            }

            /* return Error Code */
            sprintf(Error_msg, "line:%d [%c] is undefined.\n",row_num, prev_buf);
            error(Error_msg);
            token_code = -1;
            break;
        }
    }
    return token_code;
}


static int Check_Keyword(char element){
    return 1;
}

// memo:string_attrに入れ込んでないし，エラー処理してない

/* 
    Skip element of string
    Success:    TSTRING
    Error:      -1   
*/
static int Skip_String(char string_element){
    int total_string_element = 0;
    char init_character = string_element;

    /* 文字列の読み込み */
    while(string_element != EOF){
        string_element = next_buf;
        next_buf = fgetc(fp);
        /* 改行コードの扱い */
        if(string_element == '\r' || string_element == '\n') continue;
        /* 文字列が長すぎる場合のエラー判定 */
        if(total_string_element >= MAXSTRSIZE){
            sprintf(Error_msg, "line:%d [%s] is too long", row_num, string_attr);
            error(Error_msg);
            return -1;
        }
        /* 終了の判定 */
        if(string_element == (char)39){
            if(next_buf != (char)39) return TSTRING;
            string_attr[total_string_element++] = string_element;
            string_element = next_buf;
            next_buf = fgetc(fp);
        }
        string_attr[total_string_element++] = string_element;
        string_length++;
    }
    sprintf(Error_msg, "line:%d expected declaration or statement at end of input [%c]", row_num, init_character);
    error(Error_msg);
    return -1;
    
}
/*
    Skip element of attr
    Success:    TNUMBER
    Error:      -1
*/
static int Skip_Digits(char attr_element){
    int total_attr_element = 0;

    /* 数字列の読み込み */
    while((int)attr_element >= (int)'0' && (int)attr_element <= (int)'9'){
        if(total_attr_element >= MAXSTRSIZE) break;
        string_attr[total_attr_element++] = attr_element;
        attr_element = fgetc(fp);
    }

    /* num_attr */
    next_buf = attr_element;
    num_attr = atoi(string_attr);
    if(num_attr > MAXNUMSIZE){
        sprintf(Error_msg, "line:%d unsigned integer[%d] is too long(until 32767)", row_num, num_attr);
        error(Error_msg);
        return -1;
    }
    return TNUMBER;
}

/*
    Skip contents into comment
    Success:    return next character
    Error:      return -1
*/
static char Skip_Comment(char skip_character){
    char init_character = skip_character;
    skip_character = next_buf;

    while(skip_character != EOF){
        if(skip_character == '/' || skip_character == '}'){
            skip_character = fgetc(fp);
            return skip_character;
        }
        skip_character = fgetc(fp);
    }
    
    sprintf(Error_msg, "line:%d expected declaration or statement at end of input [%c]", row_num, init_character);
    error(Error_msg);
    return -1;
}

/*
    Count of a number of row
    return number of row
*/
int get_linenum(){
    return row_num;
}

void end_scan(){
    fclose(fp);
}