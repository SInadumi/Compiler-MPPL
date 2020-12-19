PROGRAM = Compiler
OBJS = main.o scan.o pretty_printer.o
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
main.o : token-list.h pretty_printer.h main.c
scan.o : token-list.h scan.c
pretty_printer.o : pretty_printer.h pretty_printer.c