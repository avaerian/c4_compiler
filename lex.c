#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

const token_kind_t TOKEN_KIND_EOF   = 0;
const token_kind_t TOKEN_KIND_IDENT = 127;
const token_kind_t TOKEN_KIND_LOGICAL_AND = 128; // &&
const token_kind_t TOKEN_KIND_LOGICAL_OR = 129; // ||
const token_kind_t TOKEN_KIND_R_ARROW = 130; // ->
const token_kind_t TOKEN_KIND_L_ARROW = 131; // <-
const token_kind_t TOKEN_KIND_COMMENT = 132; //
const token_kind_t TOKEN_KIND_L_BITSHIFT = 133; // <<
const token_kind_t TOKEN_KIND_R_BITSHIFT = 134; // >>
const token_kind_t TOKEN_KIND_NE = 135;
const token_kind_t TOKEN_KIND_EQ = 136; // ==
const token_kind_t TOKEN_KIND_LTE = 137; // <=
const token_kind_t TOKEN_KIND_GTE = 138; // >=
const token_kind_t TOKEN_KIND_INCR = 139; // ++
const token_kind_t TOKEN_KIND_DECR = 140; // --
const token_kind_t TOKEN_KIND_ADD_ASSIGN = 141; // +=
const token_kind_t TOKEN_KIND_SUB_ASSIGN = 142; // -=
const token_kind_t TOKEN_KIND_MUL_ASSIGN = 143; // *=
const token_kind_t TOKEN_KIND_DIV_ASSIGN = 144; // /=
const token_kind_t TOKEN_KIND_MOD_ASSIGN = 145; // %=
const token_kind_t TOKEN_KIND_AND_ASSIGN = 146; // &=
const token_kind_t TOKEN_KIND_OR_ASSIGN = 147; // |=

const char* SYMBOL_MAP[20] = {
    "&&",
    "||",
    "->",
    "<-",
    "//",
    "<<",
    ">>",
    "!=",
    "==",
    "<=",
    ">=",
    "++",
    "--",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "&=",
    "|=",
};

lexer_t* lexer_new(const char* src) {
    if(!src)
        return NULL;

    lexer_t* l = malloc(sizeof(lexer_t));
    if(!l) {
        printf("Failed to malloc lexer");
        return NULL;
    }
    l->code = src;
    l->cursor = 0;
    l->line = 1;
    l->col = 1;
    return l;
}

void lexer_free(lexer_t* l) {
    free(l);
}

const char* lexer_token_str(token_t t) {
    char* res;
    switch(t.kind) {
        case TOKEN_KIND_EOF:
            res = "eof";
            break;
        case TOKEN_KIND_IDENT:
            res = t.data.ident.ident;
            break;
        default:
            if(t.kind > TOKEN_KIND_IDENT) {
                return SYMBOL_MAP[t.kind - TOKEN_KIND_IDENT - 1];
            }
            res = malloc(2);
            res[0] = t.kind;
            res[1] = '\0';
            break;
    } 
    return res;
}

static inline char peek_next_char(lexer_t* l) {
    return l->code[l->cursor + 1];
}

static inline void incr_cursor(lexer_t* l) {
    l->cursor++;
    l->col++;
}

token_t lexer_new_token(lexer_t* l) {
    char kind = TOKEN_KIND_EOF;
    char c;
    uint32_t begin_row = l->line, begin_col = l->col;
    while((c = kind = l->code[l->cursor]) != '\0') { 
        switch(c) {
            case '#':
            case ':':
            case ';':
            case '{':
            case '}':
            case '[':
            case ']':
            case '(':
            case ')':
            case '@':
            case '$':
            case '\'':
            case '"':
            case '?':
            case '.':
            case '\\':
            case ',': goto ret_symbol;

            case '&':
                if(peek_next_char(l) == '&') {
                    kind = TOKEN_KIND_LOGICAL_AND;
                    incr_cursor(l);
                } else if(peek_next_char(l) == '=') {
                    kind = TOKEN_KIND_AND_ASSIGN;
                    incr_cursor(l);
                }
                goto ret_symbol;

            case '|':
                if(peek_next_char(l) == '|') {
                    kind = TOKEN_KIND_LOGICAL_OR;
                    incr_cursor(l);
                } else if(peek_next_char(l) == '=') {
                    kind = TOKEN_KIND_OR_ASSIGN;
                    incr_cursor(l);
                }
                goto ret_symbol;
            case '+':
                if(peek_next_char(l) == '+') {
                    kind = TOKEN_KIND_INCR;
                    incr_cursor(l);
                } else if(peek_next_char(l) == '=') {
                    kind = TOKEN_KIND_ADD_ASSIGN;
                    incr_cursor(l);
                }
                goto ret_symbol;
            case '-':
                switch(peek_next_char(l)) {
                    case '-':
                        kind = TOKEN_KIND_DECR;
                        goto test;
                    case '>':
                        kind = TOKEN_KIND_R_ARROW;
                        goto test;
                    case '=':
                        kind = TOKEN_KIND_SUB_ASSIGN;
                        goto test;
                    test:
                        incr_cursor(l);
                        break;
                }
                goto ret_symbol;
            case '*':
                switch(peek_next_char(l)) {
                    case '=':
                        kind = TOKEN_KIND_MUL_ASSIGN;
                        incr_cursor(l);
                        break;
                }
                goto ret_symbol;
            case '/':
                switch(peek_next_char(l)) {
                    case '/':
                        kind = TOKEN_KIND_COMMENT;
                        incr_cursor(l);
                        break;
                }
                goto ret_symbol;
            case '%':
                switch(peek_next_char(l)) {
                    case '=':
                        kind = TOKEN_KIND_MOD_ASSIGN;
                        incr_cursor(l);
                        break;
                }
                goto ret_symbol;
            case '=':
                if(peek_next_char(l) == '=') {
                    kind = TOKEN_KIND_EQ;
                    incr_cursor(l);
                }
                goto ret_symbol;
            case '!':
                if(peek_next_char(l) == '=') {
                    kind = TOKEN_KIND_NE;
                    incr_cursor(l);
                }
                goto ret_symbol;
            
            case '<':
                switch(peek_next_char(l)) {
                    case '<':
                        kind = TOKEN_KIND_L_BITSHIFT;
                        goto step;
                    case '-':
                        kind = TOKEN_KIND_L_ARROW;
                        goto step;
                    step:
                        incr_cursor(l);
                        //break;
                }
                goto ret_symbol;
            case '>':
                if(peek_next_char(l) == '>') {
                    kind = TOKEN_KIND_R_BITSHIFT;
                    incr_cursor(l);
                }
                goto ret_symbol;
            case '^':
                goto ret_symbol;

            case '\n':
                l->line++;
                l->col = 0;
            case ' ':
                //TODO: set flag for preceding whitespace to true
                l->cursor++;
                l->col++;
                break;

            default:
                // FIXME: skip over unidentified chars (check if token len is 0)
                kind = TOKEN_KIND_IDENT;
                uint32_t begin = l->cursor;
                while((c = l->code[l->cursor]) != '\0' && (isalnum(c) || c == '_')) {       
                    l->cursor++;
                    l->col++;
                }
                //printf("cursor: %d, begin: %d\n", l->cursor, begin);
                char* cpy = malloc(l->cursor - begin + 1);
                strncpy(cpy, &l->code[begin], l->cursor - begin);
                cpy[l->cursor - begin] = '\0';
                return (token_t){ .kind = kind, .row = begin_row, .col = begin_col, .data = { .ident = { .ident = cpy } } };
        }
        //l->cursor++;
    }
    return (token_t){ .kind = kind, .row = begin_row, .col = begin_col, .data = { .sym = { ._unused = c } } };

ret_symbol:
    l->cursor++;
    l->col++;
    return (token_t){ .kind = kind, .row = begin_row, .col = begin_col, .data = { .sym = { ._unused = c } } };
}
