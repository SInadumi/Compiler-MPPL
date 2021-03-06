#include "mppl_compiler.h"

/* public */
int num_attr = 0;
char string_attr[MAXSTRSIZE];

/* private */
static int Skip_Keyword(char element);
static int Check_Keyword(char *elements);
static int Skip_Digits(char attr_element);
static int Skip_String(char string_element);
static int Skip_Comment(char skip_character);
static void Reset_val(void);

static int row_num = 1;         /* number of row */
static char next_buf = '\0';    /* Look ahead buffer */

/* keyword list */
struct KEYWORDS key[KEYWORDSIZE] = {
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
int init_scan(char *filename, FILE **input){
    num_attr = 0;
    *input = fopen(filename, "r");
    if(*input == NULL){
        return error("%s can not open", filename);
    }
    next_buf = (char)fgetc(*input);
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
        
        next_buf = fgetc(input);

        if(now_buf == EOF) return -1;
        
        switch (now_buf){
        // symbol
        case '<':
            token_code = TLE;
            if(next_buf == '>') token_code = TNOTEQ;
            else if(next_buf == '=') token_code = TLEEQ;
            
            if(next_buf == '>' || next_buf == '=') next_buf = fgetc(input);
            break;

        case '>':
            token_code = TGR;
            if(next_buf == '='){
                next_buf = fgetc(input);
                token_code = TGREQ;
            }
            break;
        case ':':
            token_code = TCOLON;
            if(next_buf == '='){
                next_buf = fgetc(input);
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
            if(next_buf == '\r') next_buf = fgetc(input);
            break;
        case '\r':
            row_num++;
            if(next_buf == '\n') next_buf = fgetc(input);
            break;

        // string
        case (char)39:  
            token_code = Skip_String(now_buf);
            break;

        default:   
            /* comment */
            if((now_buf == '/' && next_buf == '*') || now_buf == '{'){
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
            error("undefined character");
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
    while(((int)element >= (int)'A' && (int)element <= (int)'Z')
       || ((int)element >= (int)'a' && (int)element <= (int)'z' )){

        if(total_word_element >= MAXSTRSIZE) break;

        string_attr[total_word_element++] = element;
        element = fgetc(input);
    
    }

    /* Input Alphabet or Digits to string_attr */
    while(((int)element >= (int)'A' && (int)element <= (int)'Z')
       || ((int)element >= (int)'a' && (int)element <= (int)'z' )
       || ((int)element >= (int)'0' && (int)element <= (int)'9')){
        
        if(total_word_element >= MAXSTRSIZE) break;

        string_attr[total_word_element++] = element;
        element = fgetc(input);

    }
    
    /* Error: Overflow */
    if(total_word_element >= MAXSTRSIZE){
        error("too long element");
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
static int Check_Keyword(char *elements){
    int index;
    for(index = 0; index < KEYWORDSIZE; index++){
        if(strcmp(key[index].keyword, elements) == 0){
            return key[index].keytoken;
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

    /* Input String to string_attr */
    while(string_element != EOF){
        string_element = next_buf;
        next_buf = fgetc(input);

        /* If Input element is '\r' or '\n' */
        if(string_element == '\r' || string_element == '\n') continue;
        
        /* Error: Overflow */
        if(total_string_element >= MAXSTRSIZE){
            error("too long element");
            return -1;
        }

        /* If Input element is ['] */
        if(string_element == (char)39){
            if(next_buf != (char)39) return TSTRING;
            string_attr[total_string_element++] = string_element;
            string_element = next_buf;
            next_buf = fgetc(input);
        }

        string_attr[total_string_element++] = string_element;
    }

    /* Error: Token hasn't statement at end of ["] */
    error("expected declaration or statement at end of input");
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
        attr_element = fgetc(input);
    }

    /* Update num_attr */
    num_attr = atoi(string_attr);
    if(num_attr > MAXNUMSIZE){
        error("unsigned integer is too long(until 32767)");
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
        next_buf = fgetc(input);
        if(skip_character == '*' && next_buf == '/' && init_character == '/'){
            next_buf = fgetc(input);
            return 0;
        }
        if(skip_character == '}' && init_character == '{'){
            return 0;
        }
    }
    error("expected declaration or statement at end of input");
    return -1;
}
/*
    Reset num_attr, string_attr, string_length
*/
static void Reset_val(){
    num_attr = 0;
    memset(string_attr, '\0', MAXSTRSIZE);
}

/*
    Count of a number of row
    return number of row
*/
int get_linenum(){
    return row_num;
}

void end_scan(FILE *input){
    fclose(input);
}