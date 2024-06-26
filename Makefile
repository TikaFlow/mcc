HEADER=defs.h protos.h
SRCS=mcc.c util.c lexer.c parser.c
EXEC=./mcc

STARTER=/lib/x86_64-linux-gnu/crt1.o
LOADER=/lib64/ld-linux-x86-64.so.2

.PHONY: all test clean

all: test

$(EXEC): $(HEADER) $(SRCS)
	gcc -o $@ -g -Wall -DDEBUG $^

test: $(EXEC)
	$(EXEC) test/test.c
	@echo "All Tests passed!"

clean:
	rm -f $(EXEC) out* *.out *.i *.s *.o test/*