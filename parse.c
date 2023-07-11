#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "9cc.h"
#include "parse.h"
#include "error.h"

extern Token *token;

//次のトークンが期待している記号のときには、トークンを一つ読み進めて
//それ意外の場合にはエラーを報告する。
void expect(char *op) {
	if(token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0) {
		error_at(token->str, "expected '%s'", op);
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

//次のトークンが期待している記号のときには、トークンを一つ読み勧めて
//真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
	if(token->kind != TK_RESERVED ||
			strlen(op) != token->len ||
			memcmp(token->str, op, token->len) != 0)
		return false;
	token = token->next;
	return true;
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
	return equality();
}

Node *equality() {
	Node *node = relational();
	for(;;) {
		if(consume("==")) {
			node = new_node(ND_EQU, node, relational());
		} else if(consume("!=")) {
			node = new_node(ND_NEQ, node, relational());
		} else {
			return node;
		}
	}
}

Node *relational() {
	Node *node = add();
	for(;;) {
		if(consume("<")) {
			node = new_node(ND_LNE, node, add());
		} else if(consume("<=")) {
			node = new_node(ND_LE, node, add());
		} else if(consume(">")) {
			node = new_node(ND_LNE, add(), node);
		} else if(consume(">=")) {
			node = new_node(ND_LE, add(), node);
		} else {
			return node;
		}
	}
}

Node *add() {
	Node *node = mul();
	for(;;) {
		if(consume("+")) {
			node = new_node(ND_ADD, node, mul());
		} else if(consume("-")) {
			node=  new_node(ND_SUB, node, mul());
		} else {
			return node;
		}
	}
}

Node *mul() {
	Node *node = unary();
	for(;;) {
		if(consume("*")) {
			node = new_node(ND_MUL, node, unary());
		} else if(consume("/")) {
			node = new_node(ND_DIV, node, unary());
		} else {
			return node;
		}
	}
}

Node *unary() {
	if(consume("+"))
		return primary();
	if(consume("-"))
		return new_node(ND_SUB, new_node_num(0),primary());
	return primary();
}

Node *primary() {
	//次のトークンが"("なら"(" expr ")"のはず
	if(consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}

	//そうでなければ数値のはず
	return new_node_num(expect_number());
}
