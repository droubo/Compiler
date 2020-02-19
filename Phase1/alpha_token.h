#ifndef ALPHA_TOKEN_H
#define ALPHA_TOKEN_H

struct alpha_token_t {
	unsigned int     numline;
	unsigned int     numToken;
	char          *content;
	char          *type;
	struct alpha_token_t *alpha_yylex;

}

#endif