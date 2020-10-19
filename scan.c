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
static int Skip_Keyword(char element);
static int Check_Keyword(char *elements);
static int Skip_Digits(char attr_element);
static int Skip_String(char string_element);
static int Skip_Comment(char skip_character);
static void Reset_val();
FILE *fp;
static int row_num = 1; /* number of row */
static char next_buf;   /* Look ahead buffer */

/* keyword list */
keywords key[KEYWORDSIZE] = {
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
    char now_buf;
    int token_code = 0;

    Reset_val();        // Initialize attr and string_length
    now_buf = next_buf;

    while(!token_code){

        now_buf = next_buf;
        
        next_buf = fgetc(fp);
        if(now_buf == EOF) return -1;
        
        switch (now_buf){
        // symbol
        case '<':
            token_code = TLE;
            if(next_buf == '>') token_code = TNOTEQ;
            else if(next_buf == '=') token_code = TLEEQ;
            
            if(next_buf == '>' || next_buf == '=') next_buf = fgetc(fp);
            break;

        case '>':
            token_code = TGR;
            if(next_buf == '='){
                next_buf = fgetc(fp);
                token_code = TGREQ;
            }
            break;
        case ':':
            token_code = TCOLON;
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
        case '\n':
            row_num++;
            if(next_buf == '\r') next_buf = fgetc(fp);
            break;
        case '\r':
            row_num++;
            if(next_buf == '\n') next_buf = fgetc(fp);
            break;

        // string
        case (char)39:  
            token_code = Skip_String(now_buf);
            break;

        default:   
            /* comment */
            if(now_buf == '/' && next_buf == '*' || now_buf == '{'){
                token_code = Skip_Comment(now_buf);
                break;
            }
            /* name or keyword */
            if((int)now_buf >= (int)'A' && (int)now_buf <= (int)'z'){
                token_code = Skip_Keyword(now_buf);
                break;
            }

            /* digit */
            if((int)now_buf >= (int)'0' && (int)now_buf <= (int)'9'){
                token_code = Skip_Digits(now_buf);
                break;
            }

            /* return Error Code */
            sprintf(Error_msg, "line:%d [%c] is undefined.\n", get_linenum(), now_buf);
            error(Error_msg);
            token_code = -1;
            break;
        }
    }
    return token_code;
}

/*
    Skip element of keyword or name
    Success:    keyword's token code or TNAME
    Error:      -1
*/
static int Skip_Keyword(char element){
    int token = TNAME;
    int total_word_element = 0;
    string_attr[total_word_element++] = element;
    element = next_buf;

    /* Input Alphabet to string_attr */
    while((int)element >= (int)'A' && (int)element <= (int)'Z'
       || (int)element >= (int)'a' && (int)element <= (int)'z' ){

        if(total_word_element >= MAXSTRSIZE) break;

        string_attr[total_word_element++] = element;
        string_length++;
        element = fgetc(fp);
    
    }

    /* Input Alphabet or Digits to string_attr */
    while((int)element >= (int)'A' && (int)element <= (int)'Z'
       || (int)element >= (int)'a' && (int)element <= (int)'z' 
       || (int)element >= (int)'0' && (int)element <= (int)'9'){
        
        if(total_word_element >= MAXSTRSIZE) break;

        string_attr[total_word_element++] = element;
        string_length++;
        element = fgetc(fp);

    }
    
    /* Error: Overflow */
    if(total_word_element >= MAXSTRSIZE){
        sprintf(Error_msg, "line:%d too long element\n[%s]", get_linenum(), string_attr);
        error(Error_msg);
        return -1;
    }

    /* Decide token of Keyword or NAME */
    token = Check_Keyword(string_attr);
    next_buf = element;
    return token;
}

/*
    Check element of keyword or name
    Success:        keyword's token code
    Error:          -1
*/
static int Check_Keyword(char* elements){
    int i;
    for(i = 0; i < KEYWORDSIZE; i++){
        if(strcmp(key[i].keyword, elements) == 0){
            return key[i].keytoken;
        }
    }
    return TNAME;
}

/* 
    Skip element of string
    Success:    TSTRING
    Error:      -1   
*/
static int Skip_String(char string_element){
    int total_string_element = 0;
    char init_character = string_element;

    /* Input String to string_attr */
    while(string_element != EOF){
        string_element = next_buf;
        next_buf = fgetc(fp);

        /* If Input element is '\r' or '\n' */
        if(string_element == '\r' || string_element == '\n') continue;
        
        /* Error: Overflow */
        if(total_string_element >= MAXSTRSIZE){
            sprintf(Error_msg, "line:%d too long element\n[%s]", get_linenum(), string_attr);
            error(Error_msg);
            return -1;
        }

        /* If Input element is ['] */
        if(string_element == (char)39){
            if(next_buf != (char)39) return TSTRING;
            string_attr[total_string_element++] = string_element;
            string_element = next_buf;
            next_buf = fgetc(fp);
        }

        string_attr[total_string_element++] = string_element;
        string_length++;
    }

    /* Error: Token hasn't statement at end of ["] */
    sprintf(Error_msg, "line:%d expected declaration or statement at end of input [%c]", get_linenum(), init_character);
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
    string_attr[total_attr_element++] = attr_element;
    attr_element = next_buf;

    /* Input Digits to string_attr */
    while((int)attr_element >= (int)'0' && (int)attr_element <= (int)'9'){
        if(total_attr_element >= MAXSTRSIZE) break;
        string_attr[total_attr_element++] = attr_element;
        attr_element = fgetc(fp);
    }

    /* Update num_attr */
    num_attr = atoi(string_attr);
    if(num_attr > MAXNUMSIZE){
        sprintf(Error_msg, "line:%d unsigned integer[%d] is too long(until 32767)", get_linenum(), num_attr);
        error(Error_msg);
        return -1;
    }

    next_buf = attr_element;
    return TNUMBER;
}

/*
    Skip contents into comment
    Success:    return 0
    Error:      return -1
*/
static int Skip_Comment(char skip_character){
    char init_character = skip_character;

    while(skip_character != EOF){
        skip_character = next_buf;
        next_buf = fgetc(fp);
        if((skip_character == '*' && next_buf == '/') || skip_character == '}'){
            next_buf = fgetc(fp);
            return 0;
        }
    }
    
    sprintf(Error_msg, "line:%d expected declaration or statement at end of input [%c]", get_linenum(), init_character);
    error(Error_msg);
    return -1;
}
/*
    Reset num_attr, string_attr, string_length
*/
static void Reset_val(){
    num_attr = 0;
    memset(string_attr, '\0', MAXSTRSIZE);
    string_length = 0;
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