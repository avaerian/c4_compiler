#include <stdlib.h>
#include <stdio.h>
#include "parse.h"

static void expect(lexer_t* l, unsigned char tok_kind, const char* err_msg) {
    token_t tok = lexer_next_token(l);
    if(tok.kind != tok_kind) {
        fprintf(stderr, "%s\n", err_msg);
        exit(1);
    }
}

void parse_expr(lexer_t* l);

void parse_statement(lexer_t* l) {
    token_t tok = lexer_next_token(l);
    if(tok.kind == TOKEN_KIND_KEYWORD) {
        switch(tok.data.keyword.kw) {
            case IF:
                {}
                char err_msg[64] = {0};
                snprintf(err_msg, 64, "Missing '(' at (row: %d, col: %d)", tok.row, tok.col);
                expect(l, '(', err_msg);
                
                parse_expr(l);
                break;
        }
    }
}

void parse_expr(lexer_t* l) {
    token_t tok = lexer_next_token(l); // ident
    switch(tok.kind) {
        case TOKEN_KIND_IDENT:
            
            break;
        
        default:
            //unexpected
            break;
    }
    char* name;
}

static int next_if(lexer_t* l, unsigned char kind) {
    uint32_t cursor = l->cursor, col = l->col, line = l->line;
    token_t tok = lexer_next_token(l);
    if(tok.kind != kind) {
        l->cursor = cursor;
        l->col = col;
        l->line = line;
    }
    return tok.kind == kind;
}

void parse_number(lexer_t* l) {
    token_t num = lexer_next_token(l);
    double n = num.data.intlit.val; //review conversion between uint64_t and double
    if(next_if(l, '.')) {
        token_t decimal = lexer_next_token(l);
        if(decimal.kind != TOKEN_KIND_INTLIT) {
            // err
        }

    }
}
