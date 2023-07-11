TARGET=9cc

CFLAGS=-std=c11 -g -static

OBJS = src/main.o src/codegen.o src/parse.o src/token.o src/error.o

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $+

%.o: src/%.c
	gcc $(CFLAGS) -c -o $@ $<

test: 9cc
	./test.sh

clean:
	rm -f 9cc ./src/*.o *~ tmp*

.PHONY: test clean
