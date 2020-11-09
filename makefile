PROGRAM = Compiler
OBJS = main.c id-list.c scan.c pretty_printer.c
CC = gcc
CFLAGS = -g -Wall -Wextra

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $^

# token-list.o: token-list.c
# 	$(CC) $(CFLAGS) -c $<

# id-list.o: id-list.c
# 	$(CC) $(CFLAGS) -c $<
# scan.o: scan.c
# 	$(CC) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJS)

# 依存関係
main.o : token-list.h pretty_printer.h main.c
id-list.o : token-list.h id-list.c
scan.o : token-list.h scan.c
pretty_printer.o : pretty_printer.h pretty_printer.c