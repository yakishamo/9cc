#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

//現在着目しているトークン
Token *token;

//入力プログラム
char *user_input;

Node *expr();
Node *mul();
Node *unary();
Node *primary();

//エラーを報告するための関数
//printfと同じ引数をとる
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " "); //pos個の空白を出力
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//次のトークンが期待している記号のときには、トークンを一つ読み勧めて
//真を返す。それ以外の場合には偽を返す。
bool consume(char op) {
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

//次のトークンが期待している記号のときには、トークンを一つ読み進めて
//それ意外の場合にはエラーを報告する。
void expect(char op) {
	if(token->kind != TK_RESERVED || token->str[0] != op) {
		error_at(token->str, "expected '%c'", op);
	}
	token = token->next;
}

//次のトークンが数値の場合、トークンを一つ読み勧めてその数値を返す。
//それ以外の場合にはエラーを報告する。
int expect_number() {
	if(token->kind != TK_NUM)
		error_at(token->str, "expected a number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
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
				*p == '(' || *p == ')') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p,&p,10);
			continue;
		} 
		
		error("invalid token");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = calloc(1,sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *expr() {
	Node *node = mul();
	for(;;) {
		if(consume('+')) {
			node = new_node(ND_ADD, node, mul());
		} else if(consume('-')) {
			node=  new_node(ND_SUB, node, mul());
		} else {
			return node;
		}
	}
}

Node *mul() {
	Node *node = unary();
	for(;;) {
		if(consume('*')) {
			node = new_node(ND_MUL, node, unary());
		} else if(consume('/')) {
			node = new_node(ND_DIV, node, unary());
		} else {
			return node;
		}
	}
}

Node *unary() {
	if(consume('+'))
		return primary();
	if(consume('-'))
		return new_node(ND_SUB, new_node_num(0),primary());
	return primary();
}

Node *primary() {
	//次のトークンが"("なら"(" expr ")"のはず
	if(consume('(')) {
		Node *node = expr();
		expect(')');
		return node;
	}

	//そうでなければ数値のはず
	return new_node_num(expect_number());
}

void gen(Node *node) {
	if(node->kind == ND_NUM) {
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->kind) {
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
	}
	printf("	push rax\n");
}

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
