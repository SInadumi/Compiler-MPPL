#include "mppl_compiler.h"

/* public */

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

void print_labelname(char *label){
    fprintf(output, "%s\n", label);
}

int print_id_label(struct ID *p){

    if(p == NULL){
        return error("ID is not found");
    }else if(p->itp == NULL){
        return error("type of ID is not defined");
    }else if(p->name == NULL){
        return error("name of ID is not defined");
    }
    
    if(p->procname != NULL) fprintf(output, "$%s%%s", p->name, p->procname);
    else fprintf(output, "$%s", p->name);

    if(p->itp->ttype == TPARRAY) fprintf(output, "\tDS\t%d\n", p->itp->arraysize);
    else fprintf(output, "\tDC\t0\n");
    return NORMAL;
}

/* Program 'start' */
int inst_start(char *program_name, char **st_label){
    fprintf(output, "$$%s\tSTART\n", program_name);
    fprintf(output, "\tLAD\tgr0,0\n");
    if(create_label(st_label) == ERROR) return ERROR;
    fprintf(output, "\tCALL\t%s\n", *st_label);
    fprintf(output, "\tCALL\tFLUSH\n");
    fprintf(output, "\tSVC\t0\n");
    return NORMAL;
}
void inst_close_program(){
    fprintf(output, "\tEND\n");
}

/*  */