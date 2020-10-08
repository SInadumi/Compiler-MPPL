#include "token-list.h"

static int cbuf;   // anticipating char buffer
static int row_num = 0;
FILE *fp;

/* 
    Initialize file
    Case : file open error -> return -1
    Case : successed :     -> return 0
 */
int init_scan(char *filename){
    fp = fopen(filename, "r");
    if(fp == NULL) return -1;
    else return 0;
}

/*
    Scan one token
    Case : contents is keyword     -> return TokenCode
    Case : content isn't keyword   -> return Name TokenCode
    Case : content is digit string -> return digit string
    Case : EOF or Error            -> return -1
*/
int scan(){
    char one_char;
    one_char = fgetc(fp);
    if(!feof(fp) < 0) return -1;
    printf("%c",one_char);
    
    return 1;
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