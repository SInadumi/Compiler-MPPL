#include "token-list.h"

/* public */
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

/* private */
static int numtoken[NUMOFTOKEN+1];		/* Token counter */
static int name_counter[MAXSTRSIZE];	/* NAME counter */
static void Print_File(char *name);

int main(int nc, char *np[]) {
	int token;

	/* File Open */
    if(nc < 2) {
		sprintf(Error_msg, "File name is not given.\n");
		error(Error_msg);
		return 0;
    }

    if(init_scan(np[1]) < 0) {
		sprintf(Error_msg, "File %s can not open.\n", np[1]);
		error(Error_msg);
		return 0;
    }

	/* Initialized array */
	for(int i = 0; i <= NUMOFTOKEN; ++i) numtoken[i] = 0;

	/* Count Token */
    while((token = scan()) >= 0) {
		numtoken[token]++;
		if(token == TNAME){
			
		}
    }

    end_scan();

    /* Print */
	Print_File(np[1]);

    return 0;
}

/*
	Output result in output/[name]
*/
void Print_File(char *name){
	FILE *output;
	char filename[MAXSTRSIZE];

	sprintf(filename, "outputs/%s",name);
	output = fopen(filename, "w");

	if(output == NULL){
		error("file doesn't exist");
		fclose(output);
		return ;
	}

	for(int i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		fprintf(output, "\"%s\t\"\t%d\n", tokenstr[i], numtoken[i]);
	}
	fclose(output);
}

void error(char *mes) {
	printf("\n ERROR:%s\n", mes);
}
