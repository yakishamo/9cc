#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "9cc.h"
#include "token.h"
#include "error.h"

extern Token *token;
extern char *user_input;

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

//入力文字列user_inputをトークナイズしてそれを返す
Token *tokenize() {
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p) {
		//空白をスキップ
		if(isspace(*p)) {
			p++;
			continue;
		}

		if(*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
				*p == '(' || *p == ')' || *p == '<' || *p == '>') {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if(strncmp(p, "<=", 2) == 0 ||
				strncmp(p, ">=", 2) == 0) {
			cur = new_token(TK_RESERVED, cur, p+=2, 2);
			continue;
		}

		if(isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			cur->val = strtol(p,&p,10);
			continue;
		} 
		
		error("invalid token");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

