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
main.o : token-list.h parser.h main.c
scan.o : token-list.h scan.c
parser.o : parser.h parser.c
cxref.o : cxref.h cxref.h