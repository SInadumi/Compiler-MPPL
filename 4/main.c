#include "mppl_compiler.h"

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

int error(const char *mes, ...) {
	va_list args;
	char out[MAXSTRSIZE];
	va_start(args, mes);
	vsnprintf(out, MAXSTRSIZE, mes, args);
	va_end(args);

	fflush(stdout);
	fprintf(stderr, "\n ERROR: line %d %s\n", get_linenum(), out);
	return ERROR;
}
