#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"
#include "codegen.h"
#include "token.h"
#include "parse.h"
#include "error.h"

//現在着目しているトークン
Token *token;
//入力プログラム
char *user_input;

int main(int argc, char **argv) {
	if(argc != 2) {
		error("%s: invalid number of arguments\n", argv[0]);
		return 1;
	}
	
	user_input = argv[1];
	token = tokenize();

	Node *top = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	
	gen(top);
	
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
