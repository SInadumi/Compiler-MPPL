#include "token-list.h"

/* keyword list */
struct KEY key[KEYWORDSIZE] = {
	{"and", 	TAND	},
	{"array",	TARRAY	},
	{"begin",	TBEGIN	},
	{"boolean",	TBOOLEAN},
	{"break",	TBREAK  },
	{"call",	TCALL	},
	{"char",	TCHAR	},
	{"div",		TDIV	},
	{"do",		TDO	},
	{"else",	TELSE	},
	{"end",		TEND	},
	{"false",	TFALSE	},
	{"if",		TIF	},
	{"integer",	TINTEGER},
	{"not",		TNOT	},
	{"of",		TOF	},
	{"or",		TOR	},
	{"procedure", TPROCEDURE},
	{"program",	TPROGRAM},
	{"read",	TREAD	},
	{"readln",	TREADLN },
	{"return", 	TRETURN },
	{"then",	TTHEN	},
	{"true",	TTRUE	},
	{"var",		TVAR	},
	{"while",	TWHILE	},
	{"write",	TWRITE  },
	{"writeln",	TWRITELN}
};

/* Token counter */
static int numtoken[NUMOFTOKEN+1];

int main(int nc, char *np[]) {
	int token;

	/* File Open */
    if(nc < 2) {
		printf("File name id not given.\n");
		return 0;
    }
    if(init_scan(np[1]) < 0) {
		printf("File %s can not open.\n", np[1]);
		return 0;
    }

	/* Initialized array */
	for(int i = 0; i < NUMOFTOKEN; i++) numtoken[i] = 0;

    while((token = scan()) >= 0) {
		numtoken[token]++;
		/*if(token == TNAME)*/
    }
	
    end_scan();

    /* Print */
	for(int i = 0; i <= NUMOFTOKEN; i++){
		if(numtoken[i] == 0) continue;
		printf("\"%s\"\t", tokenstr[i]);
		printf("%d\n", numtoken[i]);
	}

    return 0;
}

void error(char *mes) {
	printf("\n ERROR: %s\n", mes);
	end_scan();
}
