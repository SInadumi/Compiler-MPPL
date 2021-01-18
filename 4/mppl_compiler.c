#include "mppl_compiler.h"

/* private */
int labelnum = 0;

int init_outfile(char *filename, FILE **out){
    char outfilename[MAXSTRSIZE];
    char *tmpname;

    tmpname = strtok(filename, ".");
    snprintf(outfilename, MAXSTRSIZE, "%.30s.csl", tmpname);
    *out = fopen(outfilename, "w");
    if(*out == NULL){
        return error("%s can not open", outfilename);
    }
    return 0;
}

void end_outfile(FILE *out){
    fclose(out);
}

int create_label(char **label){
    char *t_label;
    if((t_label = (char *)malloc((MAXLABELSIZE) * sizeof(char))) == NULL){
        return error("can not malloc in create label");
    }
    
    labelnum++;
    if(labelnum >= 10000){
        return error("num of label is too many");
    }
    snprintf(t_label, MAXLABELSIZE, "L%04d", labelnum);
    *label = t_label;
    return NORMAL;
}

int inst_start(char *program_name, char **st_label){
    fprintf(output, "$$%s\tSTART\n", program_name);
    fprintf(output, "\tLAD\tgr0,0\n");
    if(create_label(st_label) == ERROR) return ERROR;
    fprintf(output, "\tCALL\t%s\n", *st_label);
    fprintf(output, "\tCALL\tFLUSH\n");
    fprintf(output, "\tSVC\t0\n");
    return NORMAL;
}