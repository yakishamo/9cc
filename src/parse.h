#ifndef PARSE_H
#define PARSE_H

#include "9cc.h"

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

#endif /*PARSE_H*/
