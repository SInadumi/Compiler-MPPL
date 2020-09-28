PROGRAM = Compiler
OBJS = token-list.c id-list.c scan.c
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
token-list.o : token-list.c token-list.h
id-list.o : id-list.c token-list.h
scan.o : scan.c token-list.h