PROGRAM = ../Compiler
OBJS = main.o scan.o parser.o cxref.o
CC = gcc
CFLAGS = -g -Wall -Wextra

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $^
	
.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJS)

# 依存関係
main.o : mppl_compiler.h main.c
scan.o : mppl_compiler.h scan.c
parser.o : mppl_compiler.h parser.c
cxref.o : mppl_compiler.h cxref.c