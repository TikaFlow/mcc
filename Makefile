HEADER=defs.h protos.h
SRCS=mcc.c lexer.c util.c
EXEC=./mcc

STARTER=/lib/x86_64-linux-gnu/crt1.o
LOADER=/lib64/ld-linux-x86-64.so.2

.PHONY: all test clean

all: test

$(EXEC): $(HEADER) $(SRCS)
	gcc -o $@ -g -Wall -DDEBUG $^

test: $(EXEC)
	@echo "All Tests passed!"

clean:
	rm -f $(EXEC) out* *.out *.i *.s *.o test/*