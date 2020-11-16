#include "token-list.h"
#include "pretty_printer.h"

/* public  */
/* private */

int main(int nc, char *np[]) {
	FILE *fp;
	int is_success = EXIT_SUCCESS;

	/* File Open */
    if(nc < 2) {
		error("File name is not given.");
		return 0;
    }

    if(init_scan(np[1], &fp) < 0) {
		error("File can not open.");
		return 0;
    }

	/* Parse(program) */
	is_success = Parse_program(fp);

	/*Release memory */
	end_scan(fp);
    
	return is_success;
}

int error(const char *mes) {
	printf("\n ERROR: line %d %s\n", get_linenum(), mes);
	return ERROR;
}

int warning(const char *mes) {
	printf("\n WARNING: line %d %s\n", get_linenum(), mes);
	return 0;
}
