#include "token-list.h"

static int cbuf;   // anticipating char buffer
static FILE *fp;
/* 
    Initialize file
    Case : file open error -> return 1
    Case : successed :     -> return is one character in file
    Case : EOF error :     -> return EOF code
 */
int init_scan(char *filename){
    fp = fopen(filename, "r");
    return 1;
}

/*
    Scan one token
    Case : contents is keyword     -> return TokenCode
    Case : content isn't keyword   -> return Name TokenCode
    Case : content is digit string -> return digit string
    Case : EOF                     -> return -1
*/
int scan(){
    return 1;
}

/*
    Count of a number of row
    return number of row
*/
int get_linenum(){
    return 1;
}

void end_scan(){
    fclose(fp);
}