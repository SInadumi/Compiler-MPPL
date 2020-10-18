#include "token-list.h"

/* string of each token */
const char *tokenstr[NUMOFTOKEN+1] = {
	"",
	"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
	 "else", "procedure", "return", "call", "while", "do", "not", "or", 
	"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
	 "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", 
	">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};
char Error_msg[MAXSTRSIZE];

int main(int nc, char *np[]) {
	int token;
	int numtoken[NUMOFTOKEN+1];		/* Token counter */

	/* File Open */
    if(nc < 2) {
		sprintf(Error_msg, "File name is not given.\n");
		error(Error_msg);
		return 0;
		printf("HI");
    }
    if(init_scan(np[1]) < 0) {
		sprintf(Error_msg, "File %s can not open.\n", np[1]);
		error(Error_msg);
		return 0;
    }

	/* Initialized array */
	for(int i = 0; i <= NUMOFTOKEN; ++i) numtoken[i] = 0;
    while((token = scan()) >= 0) {
		numtoken[token]++;
		/*if(token == TNAME)*/
    }
    end_scan();
	//printf("%d",get_linenum());
    /* Print */
	for(int i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		printf("\"%s\"\t", tokenstr[i]);
		printf("%d\n", numtoken[i]);
	}

    return 0;
}

void error(char *mes) {
	printf("\n ERROR:%s\n", mes);
}
