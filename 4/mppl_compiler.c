#include "mppl_compiler.h"

/* public */

/* private */
static struct SLABEL{
    char str[MAXSTRSIZE];
    struct SLABEL *nextp;
}*strlabelroot = NULL;
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

int print_id_label(struct ID *p){

    if(p == NULL){
        return error("ID is not found");
    }else if(p->itp == NULL){
        return error("type of ID is not defined");
    }else if(p->name == NULL){
        return error("name of ID is not defined");
    }
    
    if(p->procname != NULL) fprintf(output, "$%s%%%s", p->name, p->procname);
    else fprintf(output, "$%s", p->name);

    if(p->itp->ttype == TPARRAY) fprintf(output, "\tDS\t%d\n", p->itp->arraysize);
    else if(p->itp->ttype == TPPROC) fprintf(output, "\n");
    else fprintf(output, "\tDC\t0\n");
    return NORMAL;
}
void print_strlabel(){
    struct SLABEL *p;
    for(p = strlabelroot; p != NULL; p = p->nextp){
        fprintf(output, "%s", p->str);
    }
}
int register_strlabel(char *label, char *str){
    struct SLABEL *p, *q;
    if((p = (struct SLABEL *)malloc(sizeof(struct SLABEL))) == NULL){
        return error("cannot malloc in register strlabel");
    }
    snprintf(p->str, MAXSTRSIZE, "%s\tDC\t%s\n", label, str);
    p->nextp = NULL;
    if(strlabelroot == NULL){
        strlabelroot = p;
        return NORMAL;
    }

    for(q = strlabelroot; q->nextp != NULL; q = q->nextp){}
    
    q->nextp = p;
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

/* Statement 'procedule' */
void inst_procedule_params(struct PARAM *para){
    struct PARAM *p;
    if(para == NULL) return ;
    fprintf(output, "\tPOP\tgr2\n");
    for(p = para; p != NULL; p = p->nextp){
        fprintf(output, "\tPOP\tgr1\n");
        if(p->now->procname != NULL){
            fprintf(output, "\tST\tgr1,$%s%%%s\n", p->now->name, p->now->procname);
        }else{
            fprintf(output, "\tST\tgr1,$%s\n", p->now->name);
        }
    }
    fprintf(output, "\tPUSH\t0,gr2\n");
}

int inst_write_string(char *str){
    char *label;
    char t_str[MAXSTRSIZE];
    if(create_label(&label) == ERROR) return ERROR;
    fprintf(output, "\tLAD\tgr1,%s\n",label);
    fprintf(output, "\tLD\tgr2,gr0\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    snprintf(t_str, MAXSTRSIZE, "'%s'", str);
    if(register_strlabel(label, t_str) == ERROR) return ERROR;
    return NORMAL;
}

int inst_write_value(int type, int nums){
    if(nums > 0){
        fprintf(output, "\tLAD\tgr2,%d\n", num_attr);
    }else{
        fprintf(output, "\tLD\tgr2,gr0\n");
    }
    switch(type){
        case TPINT:
            fprintf(output, "\tCALL\tWRITEINT\n");
            break;
        case TPCHAR:
            fprintf(output, "\tCALL\tWRITECHAR\n");
            break;
        case TPBOOL:
            fprintf(output, "\tCALL\tWRITEBOOL\n");
            break;
        default : 
            break;
    }
    return NORMAL;
}   
void inst_read(int type){
    switch(type){
        case TPINT:
            fprintf(output, "\tCALL\tREADINT\n");
            break;
        case TPCHAR:
            fprintf(output, "\tCALL\tREADCHAR\n");
            break;
        default :
            break;
    }
}
int inst_variable(struct ID *p, int point_to_array){
    char *label = NULL;
    if(point_to_array >= 0){
        if(create_label(&label) == ERROR) return ERROR;
        // gr2 -> 配列の要素
        fprintf(output, "\tCPA\tgr2,gr0\n");
        fprintf(output, "\tJMI\tEROV\n");
        fprintf(output, "\tLAD\tgr1,%d\n", p->itp->arraysize);
        fprintf(output, "\tCPA\tgr2,gr1\n");
        fprintf(output, "\tJMI\t%s\n", label);
        fprintf(output, "\tJUMP\tEROV\n");
        fprintf(output, "%s\n", label);
        fprintf(output, "\tPOP\tgr2\n");
        fprintf(output, "\tADDA\tgr1,gr2\n");
        return NORMAL;
    }

    if(p->ispara == FORMAL_PARAM) fprintf(output, "\tLD\tgr1,");
    else fprintf(output, "\tLAD\tgr1,");
    if(p->procname != NULL) fprintf(output, "$%s%%%s\n", p->name, p->procname);
    else fprintf(output, "$%s\n", p->name);

    return NORMAL;
}

int inst_expressions(int opr, int is_opr){
    // 実引数である式が左辺値を持たないときには，主記憶中に場所を取り，そこに式の値を
    // 置いてその場所の番地を渡す．手続きの呼び出しが終わるとその場所は再利用してよい．
    char *label = NULL;
    if(opr == TNAME && is_opr == 0){
        fprintf(output, "\tPUSH\t0,gr1\n");
    }else{
        if(create_label(&label) == ERROR) return ERROR;
        fprintf(output, "\tLAD\tgr2,%s\n", label);
        fprintf(output, "\tST\tgr1,0,gr2\n");
        fprintf(output, "\tPUSH\t0,gr2\n");
        register_strlabel(label, "0");
    }
    return NORMAL;
}

void inst_assignment(int is_subproc, struct ID *ref_id){
    if(ref_id->ispara == FORMAL_PARAM && is_subproc == 1){
        fprintf(output, "\tPOP\tgr2\n");
        fprintf(output, "\tST\tgr1,0,gr2\n");
    }else{
        if(ref_id->itp->ttype == TPARRAY){
            fprintf(output, "\tPOP\tgr2\n");
        }
        fprintf(output, "\tST\tgr1,$%s", ref_id->name);
        if(ref_id->procname != NULL){
            fprintf(output, "%%%s",ref_id->procname);
        }
        if(ref_id->itp->ttype == TPARRAY){
            fprintf(output, ",gr2");
        }
        fprintf(output, "\n");
    }
}

int inst_expression(int opr){
    char *label_t = NULL, *label_f = NULL;

    if(create_label(&label_t) == ERROR) return ERROR;
    if(create_label(&label_f) == ERROR) return ERROR;

    fprintf(output, "\tPOP\tgr2\n");
    fprintf(output, "\tCPA\tgr2,gr1\n");
    switch(opr){
        case TEQUAL:
            fprintf(output, "\tJZE\t%s\n", label_t);
            fprintf(output, "\tLD\tgr1,gr0\n");
            break;
        case TNOTEQ:
            fprintf(output, "\tJNZ\t%s\n", label_t);
            fprintf(output, "\tLD\tgr1,gr0\n");
            break;
        case TLE:
            fprintf(output, "\tJMI\t%s\n", label_t);
            fprintf(output, "\tLD\tgr1,gr0\n");
            break;
        case TLEEQ:
            fprintf(output, "\tJPL\t%s\n", label_t);
            fprintf(output, "\tLAD\tgr1,1\n");
            break;
        case TGR:
            fprintf(output, "\tJPL\t%s\n", label_t);
            fprintf(output, "\tLD\tgr1,gr0\n");
            break;
        case TGREQ:
            fprintf(output, "\tJMI\t%s\n", label_t);
            fprintf(output, "\tLAD\tgr1,1\n");
            break;
        default:
            break;
    }

    fprintf(output, "\tJUMP\t%s\n", label_f);
    fprintf(output, "%s\n", label_t);
    if(opr == TLEEQ || opr == TGREQ){
        fprintf(output, "\tLD\tgr1,gr0\n");
    }else
    {
        fprintf(output, "\tLAD\tgr1,1\n");
    }
    fprintf(output, "%s\n", label_f);
    return NORMAL;
}
void inst_minus(){
    fprintf(output, "\tLAD\tgr2,-1\n");
    fprintf(output, "\tMULA\tgr1,gr2\n");
    fprintf(output, "\tJOV\tEOVF\n");
}

void inst_simple_expression(int opr){
    fprintf(output, "\tPOP\tgr2\n");
    switch(opr){
        case TPLUS:
            fprintf(output, "\tADDA\tgr1,gr2\n");
            fprintf(output, "\tJOV\tEOVF\n");
            break;
        case TMINUS:
            fprintf(output, "\tSUBA\tgr2,gr1\n");
            fprintf(output, "\tJOV\tEOVF\n");
            fprintf(output, "\tLD\tgr1,gr2\n");
            break;
        case TOR:
            fprintf(output, "\tOR\tgr1,gr2\n");
            break;
        default:
            break;
    }
}

void inst_term(int opr){
    fprintf(output, "\tPOP\tgr2\n");
    switch(opr){
        case TSTAR:
            fprintf(output, "\tMULA\tgr1,gr2\n");
            fprintf(output, "\tJOV\tEOVF\n");
            break;
        case TAND:
            fprintf(output, "\tAND\tgr1,gr2\n");
            break;
        case TDIV:
            fprintf(output, "\tDIVA\tgr2,gr1\n");
            fprintf(output, "\tJOV\tE0DIV\n");
            fprintf(output, "\tLD\tgr1,gr2\n");
            break;
        default:
            break;
    }
}

int inst_factor_char(int type, int exp_type){
    char *label = NULL;
    switch(exp_type){
        case TPINT:
            switch(type){
                case TPINT:
                    break;
                case TPBOOL:
                    if(create_label(&label) == ERROR) return ERROR;
                    fprintf(output, "\tCPA\tgr1, gr0\n");
                    fprintf(output, "\tJZE\t%s\n", label);
                    fprintf(output, "\tLAD\tgr1,1\n");
                    fprintf(output, "%s\n", label);
                    break;
                case TPCHAR:
                    fprintf(output, "\tLAD\tgr2,127\n");
                    fprintf(output, "\tAND\tgr1,gr2\n");
                    break;
                default:
                    break;
            }
            break;
        case TPCHAR:
            switch(type){
                case TPINT:
                    break;
                case TPBOOL:
                    if(create_label(&label) == ERROR) return ERROR;
                    fprintf(output, "\tCPA\tgr1,gr0\n");
                    fprintf(output, "\tJZE\t%s\n", label);
                    fprintf(output, "\tLAD\tgr1,1\n");
                    fprintf(output, "%s\n", label);
                    break;
                case TPCHAR:
                    break;
                default:
                    break;
            }
            break;
        case TPBOOL:
            switch(type){
                case TPINT:
                    break;
                case TPBOOL:
                    break;
                case TPCHAR:
                    if(create_label(&label) == ERROR) return ERROR;
                    fprintf(output, "\tCPA\tgr1,gr0");
                    fprintf(output, "\tJZE\t%s\n", label);
                    fprintf(output, "\tLAD\tgr1,1\n");
                    fprintf(output, "%s\n", label);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return NORMAL;
}

/* Instruction CASL2 */
void INSTRUCTIONS(){
    // EOVF 201
    fprintf(output, "EOVF\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tLAD\tgr1,EOVF1\n");
    fprintf(output, "\tLD\tgr2,gr0\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tSVC\t1\t;\toverflow error stop\n");
    // EOVF1 202
    fprintf(output, "EOVF1\tDC\t'***** Run-Time Error : Overflow *****'\n");
    // E0DIV 203
    fprintf(output, "E0DIV\n");
    fprintf(output, "\tJNZ\tEOVF\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tLAD\tgr1,E0DIV1\n");
    fprintf(output, "\tLD\tgr2,gr0\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tSVC\t2\t;\t0-divide error stop\n");
    // E0DIV1 204
    fprintf(output, "E0DIV1\tDC\t'***** Run-Time Error : Zero-Divide *****'\n");
    // EROV  205
    fprintf(output, "EROV\n");
    fprintf(output, "\tCALL \tWRITELINE\n");
    fprintf(output, "\tLAD\tgr1,EROV1\n");
    fprintf(output, "\tLD\tgr2,gr0\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tSVC\t3\t;\trange-over error stop\n");
    // EROV1 206
    fprintf(output, "EROV1\tDC\t'***** Run-Time ERROR : Range-Over in Array Index *****'\n");
    // WRITECHAR 207
    fprintf(output, "WRITECHAR\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr6,SPACE\n");
    fprintf(output, "\tLD\tgr7,OBUFSIZE\n");
    fprintf(output, "WC1\n");
    fprintf(output, "\tSUBA\tgr2,ONE\n");
    fprintf(output, "\tJZE\tWC2\n");
    fprintf(output, "\tJMI\tWC2\n");
    fprintf(output, "\tST\tgr6,OBUF,gr7\n");
    fprintf(output, "\tCALL\tBOVFCHECK\n");
    fprintf(output, "\tJUMP\tWC1\n");
    fprintf(output, "WC2\n");
    fprintf(output, "\tST\tgr1,OBUF,gr7\n");
    fprintf(output, "\tCALL\tBOVFCHECK\n");
    fprintf(output, "\tST\tgr7,OBUFSIZE\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");
    // #define WRITESTR 208
    fprintf(output, "WRITESTR\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr6,gr1\n");
    fprintf(output, "WS1\n");
    fprintf(output, "\tLD\tgr4,0,gr6\n");
    fprintf(output, "\tJZE\tWS2\n");
    fprintf(output, "\tADDA\tgr6,ONE\n");
    fprintf(output, "\tSUBA\tgr2,ONE\n");
    fprintf(output, "\tJUMP\tWS1\n");
    fprintf(output, "WS2\n");
    fprintf(output, "\tLD\tgr7,OBUFSIZE\n");
    fprintf(output, "\tLD\tgr5,SPACE\n");
    fprintf(output, "WS3\n");
    fprintf(output, "\tSUBA\tgr2,ONE\n");
    fprintf(output, "\tJMI\tWS4\n");
    fprintf(output, "\tST\tgr5,OBUF,gr7\n");
    fprintf(output, "\tCALL\tBOVFCHECK\n");
    fprintf(output, "\tJUMP\tWS3\n");
    fprintf(output, "WS4\n");
    fprintf(output, "\tLD\tgr4,0,gr1\n");
    fprintf(output, "\tJZE\tWS5\n");
    fprintf(output, "\tST\tgr4,OBUF,gr7\n");
    fprintf(output, "\tADDA \tgr1,ONE\n");
    fprintf(output, "\tCALL\tBOVFCHECK\n");
    fprintf(output, "\tJUMP\tWS4\n");
    fprintf(output, "WS5\n");
    fprintf(output, "\tST\tgr7,OBUFSIZE\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // BOVFCHECK 209
    fprintf(output, "BOVFCHECK\n");
    fprintf(output, "\tADDA\tgr7,ONE\n");
    fprintf(output, "\tCPA\tgr7,BOVFLEVEL\n");
    fprintf(output, "\tJMI\tBOVF1\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "\tLD\tgr7,OBUFSIZE\n");
    fprintf(output, "BOVF1\n");
    fprintf(output, "\tRET\n");

    // BOVFLEVEL 210
    fprintf(output, "BOVFLEVEL\tDC\t256\n");

    // WRITEINT 211
    fprintf(output, "WRITEINT\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr7,gr0\n");
    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJPL\tWI1\n");
    fprintf(output, "\tJZE\tWI1\n");
    fprintf(output, "\tLD\tgr4,gr0\n");
    fprintf(output, "\tSUBA\tgr4,gr1\n");
    fprintf(output, "\tCPA\tgr4,gr1\n");
    fprintf(output, "\tJZE\tWI6\n");
    fprintf(output, "\tLD\tgr1,gr4\n");
    fprintf(output, "\tLD\tgr7,ONE\n");
    fprintf(output, "WI1\n");
    fprintf(output, "\tLD\tgr6,SIX\n");
    fprintf(output, "\tST\tgr0,INTBUF,gr6\n");
    fprintf(output, "\tSUBA\tgr6,ONE\n");
    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJNZ\tWI2\n");
    fprintf(output, "\tLD \tgr4,ZERO\n");
    fprintf(output, "\tST\tgr4,INTBUF,gr6\n");
    fprintf(output, "\tJUMP\tWI5\n");
    fprintf(output, "WI2\n");
    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJZE\tWI3\n");
    fprintf(output, "\tLD\tgr5,gr1\n");
    fprintf(output, "\tDIVA\tgr1,TEN\n");
    fprintf(output, "\tLD\tgr4,gr1\n");
    fprintf(output, "\tMULA\tgr4,TEN\n");
    fprintf(output, "\tSUBA\tgr5,gr4\n");
    fprintf(output, "\tADDA\tgr5,ZERO\n");
    fprintf(output, "\tST\tgr5,INTBUF,gr6\n");
    fprintf(output, "\tSUBA\tgr6,ONE\n");
    fprintf(output, "\tJUMP\tWI2\n");
    fprintf(output, "WI3\n");
    fprintf(output, "\tCPA\tgr7,gr0\n");
    fprintf(output, "\tJZE\tWI4\n");
    fprintf(output, "\tLD\tgr4,MINUS\n");
    fprintf(output, "\tST\tgr4,INTBUF,gr6\n");
    fprintf(output, "\tJUMP\tWI5\n");
    fprintf(output, "WI4\n");
    fprintf(output, "\tADDA\tgr6,ONE\n");
    fprintf(output, "WI5\n");
    fprintf(output, "\tLAD\tgr1,INTBUF,gr6\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");
    fprintf(output, "WI6\n");
    fprintf(output, "\tLAD\tgr1,MMINT\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // MMINT 212
    fprintf(output, "MMINT\tDC\t'-32768'\n");

    // WRITEBOOL 213
    fprintf(output, "WRITEBOOL\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tCPA\tgr1,gr0\n");
    fprintf(output, "\tJZE\tWB1\n");
    fprintf(output, "\tLAD\tgr1,WBTRUE\n");
    fprintf(output, "\tJUMP\tWB2\n");
    fprintf(output, "WB1\n");
    fprintf(output, "\tLAD\tgr1,WBFALSE\n");
    fprintf(output, "WB2\n");
    fprintf(output, "\tCALL\tWRITESTR\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // WBTRUE 214
    fprintf(output, "WBTRUE\tDC 'TRUE'\n");
    // WBFALSE 215
    fprintf(output, "WBFALSE\tDC 'FALSE'\n");
    // WRITELINE 216
    fprintf(output, "WRITELINE\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr7,OBUFSIZE\n");
    fprintf(output, "\tLD\tgr6,NEWLINE\n");
    fprintf(output, "\tST\tgr6,OBUF,gr7\n");
    fprintf(output, "\tADDA\tgr7,ONE\n");
    fprintf(output, "\tST\tgr7,OBUFSIZE\n");
    fprintf(output, "\tOUT\tOBUF,OBUFSIZE\n");
    fprintf(output, "\tST\tgr0,OBUFSIZE\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // FLUSH 217
    fprintf(output, "FLUSH\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr7,OBUFSIZE\n");
    fprintf(output, "\tJZE\tFL1\n");
    fprintf(output, "\tCALL\tWRITELINE\n");
    fprintf(output, "FL1\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // READCHAR 218
    fprintf(output, "READCHAR\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "\tLD\tgr5,RPBBUF\n");
    fprintf(output, "\tJZE\tRC0\n");
    fprintf(output, "\tST\tgr5,0,gr1\n");
    fprintf(output, "\tST\tgr0,RPBBUF\n");
    fprintf(output, "\tJUMP\tRC3\n");
    fprintf(output, "RC0\n");
    fprintf(output, "\tLD\tgr7,INP\n");
    fprintf(output, "\tLD\tgr6,IBUFSIZE\n");
    fprintf(output, "\tJNZ\tRC1\n");
    fprintf(output, "\tIN\tIBUF,IBUFSIZE\n");
    fprintf(output, "\tLD\tgr7,gr0\n");
    fprintf(output, "RC1\n");
    fprintf(output, "\tCPA\tgr7,IBUFSIZE\n");
    fprintf(output, "\tJNZ\tRC2\n");
    fprintf(output, "\tLD\tgr5,NEWLINE\n");
    fprintf(output, "\tST\tgr5,0,gr1\n");
    fprintf(output, "\tST\tgr0,IBUFSIZE\n");
    fprintf(output, "\tST\tgr0,INP\n");
    fprintf(output, "\tJUMP\tRC3\n");
    fprintf(output, "RC2\n");
    fprintf(output, "\tLD\tgr5,IBUF,gr7\n");
    fprintf(output, "\tADDA\tgr7,ONE\n");
    fprintf(output, "\tST\tgr5,0,gr1\n");
    fprintf(output, "\tST\tgr7,INP\n");
    fprintf(output, "RC3\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");
    // READINT 219
    fprintf(output, "READINT\n");
    fprintf(output, "\tRPUSH\n");
    fprintf(output, "RI1\n");
    fprintf(output, "\tCALL\tREADCHAR\n");
    fprintf(output, "\tLD\tgr7,0,gr1\n");
    fprintf(output, "\tCPA\tgr7,SPACE\n");
    fprintf(output, "\tJZE\tRI1\n");
    fprintf(output, "\tCPA\tgr7,TAB\n");
    fprintf(output, "\tJZE\tRI1\n");
    fprintf(output, "\tCPA\tgr7,NEWLINE\n");
    fprintf(output, "\tJZE\tRI1\n");
    fprintf(output, "\tLD\tgr5,ONE\n");
    fprintf(output, "\tCPA\tgr7,MINUS\n");
    fprintf(output, "\tJNZ\tRI4\n");
    fprintf(output, "\tLD\tgr5,gr0\n");
    fprintf(output, "\tCALL\tREADCHAR\n");
    fprintf(output, "\tLD\tgr7,0,gr1\n");
    fprintf(output, "RI4\n");
    fprintf(output, "\tLD\tgr6,gr0\n");
    fprintf(output, "RI2\n");
    fprintf(output, "\tCPA\tgr7,ZERO\n");
    fprintf(output, "\tJMI\tRI3\n");
    fprintf(output, "\tCPA\tgr7,NINE\n");
    fprintf(output, "\tJPL\tRI3\n");
    fprintf(output, "\tMULA\tgr6,TEN\n");
    fprintf(output, "\tADDA\tgr6,gr7\n");
    fprintf(output, "\tSUBA\tgr6,ZERO\n");
    fprintf(output, "\tCALL\tREADCHAR\n");
    fprintf(output, "\tLD\tgr7,0,gr1\n");
    fprintf(output, "\tJUMP\tRI2\n");
    fprintf(output, "RI3\n");
    fprintf(output, "\tST\tgr7,RPBBUF\n");
    fprintf(output, "\tST\tgr6,0,gr1\n");
    fprintf(output, "\tCPA\tgr5,gr0\n");
    fprintf(output, "\tJNZ\tRI5\n");
    fprintf(output, "\tSUBA\tgr5,gr6\n");
    fprintf(output, "\tST\tgr5,0,gr1\n");
    fprintf(output, "RI5\n");
    fprintf(output, "\tRPOP\n");
    fprintf(output, "\tRET\n");

    // READLINE 220
    fprintf(output, "READLINE\n");
    fprintf(output, "\tST\tgr0,IBUFSIZE\n");
    fprintf(output, "\tST\tgr0,INP\n");
    fprintf(output, "\tST\tgr0,RPBBUF\n");
    fprintf(output, "\tRET\n");

    // ONE 221
    fprintf(output, "ONE\tDC\t1\n");
    // SIX 222
    fprintf(output, "SIX\tDC\t6\n");
    // TEN 223
    fprintf(output, "TEN\tDC\t10\n");
    // SPACE 224
    fprintf(output, "SPACE\tDC\t#0020\n");
    // MINUS 225
    fprintf(output, "MINUS\tDC\t#002D\n");
    // TAB 226
    fprintf(output, "TAB\tDC\t#0009\n");
    // ZERO 227
    fprintf(output, "ZERO\tDC\t#0030\n");
    // NINE 228
    fprintf(output, "NINE\tDC\t#0039\n");
    // NEWLINE 229
    fprintf(output, "NEWLINE\tDC\t#000A\n");
    // INTBUF 230
    fprintf(output, "INTBUF\tDS\t8\n");
    // OBUFSIZE 231
    fprintf(output, "OBUFSIZE\tDC\t0\n");
    // IBUFSIZE 232
    fprintf(output, "IBUFSIZE\tDC\t0\n");
    // INP 233
    fprintf(output, "INP\tDC\t0\n");
    //OBUF 234
    fprintf(output, "OBUF\tDS\t257\n");
    // IBUF 235
    fprintf(output, "IBUF\tDS\t257\n");
    // RPBBUF 236
    fprintf(output, "RPBBUF\tDC\t0\n");
}