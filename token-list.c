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
    Print_File(np[1]);
	
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


/*

#include "token-list.h"

/* public *
/* string of each token *
const char *tokenstr[NUMOFTOKEN+1] = {
	"",
	"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
	 "else", "procedure", "return", "call", "while", "do", "not", "or", 
	"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
	 "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", 
	">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};
char Error_msg[MAXSTRSIZE];

/* private *
static int numtoken[NUMOFTOKEN+1];		/* Token counter *
static counter name_counter[MAXSTRSIZE];	/* NAME counter *
static int Count_Name(char *t_name, int size);
static void Print_File(char *name);

int main(int nc, char *np[]) {
  	int token,i;
	int total_name = 0;		// number of varity of NAME

	/* File Open *
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

	/* Initialized array *
	for(i = 0; i <= NUMOFTOKEN; ++i) numtoken[i] = 0;

	/* Count Token *
    while((token = scan()) >= 0) {
		numtoken[token]++;
		
		if(token == TNAME && total_name < MAXSTRSIZE){
			total_name = Count_Name(string_attr, total_name);
		}else if(token == TNAME && total_name >= MAXSTRSIZE){
			sprintf(Error_msg, "line:%d NAME element variety is too large [%s]\n", get_linenum(), string_attr);
        	warning(Error_msg);
		}
    }

    end_scan();

    /* Print *
    Print_File(np[1]);

    return 0;
}

/*
	Add [t_name] in num_counter
*
int Count_Name(char *t_name, int size){
  	int i;

	for(i = 0; i < size; i++){
		if(strcmp(name_counter[i].name, t_name) == 0){
			name_counter[i].num++;
			return size;
		}
	}
	memcpy(name_counter[size].name, t_name, sizeof(t_name));
	name_counter[size].num = 1;
	size++;
	printf("(%d,%s),(%d)\n",name_counter[size].num, name_counter[size].name, size);
	return size;
}

/*
	Output result in output/[name]
*
void Print_File(char *name){
  	int i;
	FILE *output;
	char filename[MAXSTRSIZE];

	//sprintf(filename, "outputs/%s",name);
	output = fopen("output.txt", "w");

	if(output == NULL){
		error("file doesn't exist");
		fclose(output);
		return ;
	}

	for(i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		fprintf(output, "\"%s\t\"\t%d\n", tokenstr[i], numtoken[i]);
	}
	fprintf(output, "\nNAME\n");
	for(i = 0; i <= MAXSTRSIZE; i++){
		if(name_counter[i].num != 0){
			fprintf(output, "\"%s\t\"\t%d\n", name_counter[i].name, name_counter[i].num);
		}
	}
	fclose(output);
}

void error(char *mes) {
	printf("\n ERROR:%s\n", mes);
}

void warning(char *mes) {
	printf("\n WARNING:%s\n", mes);
}

*/