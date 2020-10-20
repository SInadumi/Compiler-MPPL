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
static void Print_File(char *name);

int main(int nc, char *np[]) {
  	int token,i;
	int total_name = 0;		// number of varity of NAME

	/* Initialize name list */
	init_idtab();

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
	for(i = 0; i <= NUMOFTOKEN; ++i) numtoken[i] = 0;

	/* Count Token */
    while((token = scan()) >= 0) {
		numtoken[token]++;
		
		if(token == TNAME){
			id_countup(string_attr);
		}

    }

    /* Print */
    for(i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		fprintf(stdout, "\t\"%s\"\t%d\n", tokenstr[i], numtoken[i]);
	}
	print_idtab();
	//Print_File(np[1]);
	
	/*Release memory */
	release_idtab();
	end_scan();
    
	return 0;
}

/*
	Output result in output/[name]
*/
void Print_File(char *name){
  	int i;
	FILE *output;
	char filename[MAXSTRSIZE];

	/* Open [output.txt] */
	// filename = strtok();
	//sprintf(filename, "Outputs/%s",name);
	output = fopen("output.txt", "w");

	if(output == NULL){
		error("file doesn't exist");
		fclose(output);
		return ;
	}

	for(i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		fprintf(output, "\t\"%s\"\t%d\n", tokenstr[i], numtoken[i]);
	}
	print_idtab_in_File(output);
	fclose(output);
}

void error(char *mes) {
	printf("\n ERROR:%s\n", mes);
}

void warning(char *mes) {
	printf("\n WARNING:%s\n", mes);
}
