#include "token-list.h"

/*static int Check_keyword(char *word);
static void Skip_Element(char *element);*/
static char Skip_Comment(char skip_character);
static int row_num = 1;
FILE *fp;
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
        
        if(!prev_buf) return -1;
        next_buf = fgetc(fp);

        switch (prev_buf){
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
        case '/':
            next_buf = Skip_Comment(prev_buf);
            if(next_buf < 0) token_code = -1;
            break;
        case '\n':
            row_num++;
            if(next_buf == '\r') next_buf = fgetc(fp);
            break;
        case '\r':
            row_num++;
            if(next_buf == '\n') next_buf = fgetc(fp);
            break;

        // 文字列要素の場合
        //case '"':

        default:
            /* 名前・キーワードの場合 */
            if((int)prev_buf >= (int)'A' && (int)prev_buf <= (int)'z'){
                
            }
            /* 数字要素の場合 */
            if((int)prev_buf >= (int)'0' && (int)prev_buf <= (int)'9'){
                //Skip_Element(prev_buf);
                return TNUMBER;
            }
            /* return Error Code */
            sprintf(Error_msg, "line:%d [%c] is undefined.\n",row_num, prev_buf);
            error(Error_msg);
            token_code = -1;
            break;
        }
        prev_buf = next_buf;
    }
    return token_code;
}


/*static int Check_keyword(char *word){
    return 1;
}
static void Skip_Element(char *element){
    
}*/

static char Skip_Comment(char skip_character){
    char init_character = skip_character;
    skip_character = next_buf;

    while(!feof(fp)){
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