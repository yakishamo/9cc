TARGET=9cc

CFLAGS=-std=c11 -g -static

OBJS=main.o codegen.o parse.o token.o error.o

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
