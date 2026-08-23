#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdalign.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"
#include "utils.h"

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

const char* KEYWORD_MAP[KEYWORD_LEN] = {
    [FOR] = "for",
    [WHILE] = "while",
    [FN] = "fn",
    [IF] = "if",
    [ELSE] = "else",
   
    [PUB] = "pub",
    [MUT] = "mut",
    [STRUCT] = "struct",
    [ENUM] = "enum",
    [UNION] = "union",
    [RETURN] = "return",

    [CHAR] = "char",
    [U0] = "u0",
    [U8] = "u8",
    [U16] = "u16",
    [U32] = "u32",
    [U64] = "u64",
    [USIZE] = "usize",
   
    [I8] = "i8",
    [I16] = "i16",
    [I32] = "i32",
    [I64] = "i64",
    [ISIZE] = "isize",
   
    [F32] = "f32",
    [F64] = "f64",

    [VOID] = "void",

    [CONST] = "const",
    [EXTERN] = "extern",
    [STATIC] = "static",
    [ASSERT] = "assert",
    [USE] = "use",
    [IMPORT] = "import",

    [SWITCH] = "switch",
    [CASE] = "case",
    [BREAK] = "break",
    [CONTINUE] = "continue",
};

lexer_t* lexer_new(char* src, arena_t* str_alloc) {
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
    l->str_alloc = str_alloc;
    return l;
}

void lexer_free(lexer_t* l) {
    l->code = NULL;
    l->str_alloc = NULL;
    free(l);
}

const char* kw_str(kw_t kw) {
    return KEYWORD_MAP[kw];
}

const char* lexer_token_str(token_t t, arena_t* str_alloc) {
    char* res;
    switch(t.kind) {
        case TOKEN_KIND_EOF:
            res = "eof";
            break;
        case TOKEN_KIND_IDENT:
            res = arena_alloc(str_alloc, t.data.ident.len + 1);
            if(!res)
                return NULL;
            strncpy(res, t.data.ident.ident, t.data.ident.len);
            res[t.data.ident.len] = '\0';
            break;
        case TOKEN_KIND_COMMENT:
            res = arena_alloc(str_alloc, t.data.comment.len + 1);
            if(!res)
                return NULL;
            //res = malloc(t.data.comment.len + 1);  
            strncpy(res, t.data.comment.comment, t.data.comment.len);
            res[t.data.comment.len] = '\0';
            break;
        case TOKEN_KIND_KEYWORD:
            {}
            int len = strlen(kw_str(t.data.keyword.kw));
            res = arena_alloc(str_alloc, 3 + len);
            //res = malloc(3 + strlen(kw_str(t.data.keyword.kw)));
            res[0] = 'k';
            res[1] = 'w';
            res[2] = '=';
            strncpy(res + 3, kw_str(t.data.keyword.kw), len);
            break;
        case TOKEN_KIND_STRLIT:
            //TODO: refactor this mess
            res = arena_alloc(str_alloc, t.data.strlit.len + 1 + 2);
            //res = malloc(t.data.strlit.len + 1 + 2); //1 for null terminator, 2 for quotes
            strncpy(res + 1, t.data.strlit.str, t.data.strlit.len);
            res[0] = res[t.data.strlit.len + 1] = t.data.strlit.quote;
            res[t.data.strlit.len + 2] = '\0';
            break;
        case TOKEN_KIND_INTLIT:
            {}
            len = 0;
            uint64_t tmp = t.data.intlit.val;
            
            do {
                len++;
                tmp /= 10;
            } while(tmp != 0);

            tmp = t.data.intlit.val;
            res = arena_alloc(str_alloc, len + 1);
            //res = malloc(len + 1);
            for(int i = len - 1; i >= 0; i--) {
                res[i] = '0' + (tmp % 10);
                tmp /= 10;
            }
            res[len] = '\0';
            break;
        default:
            if(t.kind > TOKEN_KIND_IDENT) {
                return SYMBOL_MAP[t.kind - TOKEN_KIND_IDENT - 1];
            }
            res = arena_alloc(str_alloc, 2);
            //res = malloc(2);
            res[0] = t.kind;
            res[1] = '\0';
            break;
    } 
    return res;
}

#define NOT_A_KEYWORD -1
static int get_keyword(str_slice_t str) {
    for(int i = 0; i < KEYWORD_LEN; i++) {
        if(!strncmp(str.str, KEYWORD_MAP[i], str.len)) {
            return i;
        }
    }
    return NOT_A_KEYWORD;
}

static inline char peek_next_char(lexer_t* l) {
    return l->code[l->cursor + 1];
}

static inline void incr_cursor(lexer_t* l) {
    l->cursor++;
    l->col++;
}

token_t lexer_peek_token(lexer_t* l) {
    uint32_t cursor = l->cursor, line = l->line, col = l->col;
    token_t ret = lexer_next_token(l);
    l->cursor = cursor;
    l->line = line;
    l->col = col;
    return ret;
}

token_t lexer_next_token(lexer_t* l) {
    unsigned char kind = TOKEN_KIND_EOF;
    unsigned char c;
    uint16_t flags = 0;
    uint32_t begin_row = l->line, begin_col = l->col, begin_cursor = l->cursor;
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
            case '?':
            case '.':
            case '\\':
            case '~':
            case ',': goto ret_symbol;
            
            case '\'':
            case '"':
                kind = TOKEN_KIND_STRLIT;
                char quote = c;
                incr_cursor(l);
                while((c = l->code[l->cursor]) != '\0' && c != quote) {
                    if(c == '\\') {
                        incr_cursor(l);
                    }
                    incr_cursor(l);
                }
                incr_cursor(l); //skip quote
                return (token_t){ kind, begin_row, begin_col, flags, { .strlit = { &l->code[begin_cursor + 1], l->cursor - begin_cursor - 2, quote } } };
            
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
                        goto incr;
                    case '>':
                        kind = TOKEN_KIND_R_ARROW;
                        goto incr;
                    case '=':
                        kind = TOKEN_KIND_SUB_ASSIGN;
                        goto incr;
                    incr:
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
                        l->cursor += 2;
                        l->col += 2;
                        while(l->code[l->cursor] && l->code[l->cursor] != '\n')
                            incr_cursor(l);
                        return (token_t){ kind, begin_row, begin_col, flags, { .comment = { .type = SINGLE_LINE, .comment = &l->code[begin_cursor], .len = l->cursor - begin_cursor } } };
                    case '*':
                        kind = TOKEN_KIND_COMMENT;
                        l->cursor += 2;
                        l->col += 2;
                        while(l->code[l->cursor] 
                                && !(l->code[l->cursor] == '*' && l->code[l->cursor + 1] == '/')) {
                            //printf("DEBUG: %c%c\n", l->code[l->cursor], l->code[l->cursor+1]);
                            if(l->code[l->cursor] == '\n') {
                                l->line++;
                                l->col = 0;
                            }
                            incr_cursor(l);
                        }
                        l->cursor += 2;
                        l->col += 2;
                        printf("DEBUG: comment len = %d\n", l->cursor - begin_cursor);
                        return (token_t){ kind, begin_row, begin_col, flags, { .comment = { .type = MULTI_LINE, .comment = &l->code[begin_cursor], .len = l->cursor - begin_cursor } } };
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
                if(c == ' ')
                    flags |= PREC_WHITESPACE;
                l->cursor++;
                l->col++;
                begin_row = l->line;
                begin_col = l->col;
                begin_cursor = l->cursor;
                break;

            case '0' ... '9':
                //NOTE: this is intended to be used for the lexer tokenizing, rather than
                //what the actual result could be from parsing; for example, 1f (default to f64), 0.5f32, 0.5f64.
                //In this case, f32 will be the next token in the stream with the bit flag PREC_WHITESPACE unset
                //after the potential int/float literal.
                kind = TOKEN_KIND_INTLIT; // could be float literal
                uint64_t val = 0;
                while((c = l->code[l->cursor]) != '\0' && isdigit(c)) {
                    val = (val * 10) + (c - '0'); 
                    incr_cursor(l);
                } 
                return (token_t){ kind, begin_row, begin_col, flags, { .intlit = { val } } };

            default:
                kind = TOKEN_KIND_IDENT;
                while((c = l->code[l->cursor]) != '\0' && (isalnum(c) || c == '_')) {       
                    l->cursor++;
                    l->col++;
                }
                if(l->cursor - begin_cursor == 0) {
                    kind = TOKEN_KIND_ERR;
                    //skip char and go to next
                    l->cursor++;
                    l->col++;
                    return (token_t){ kind, begin_row, begin_col, flags, { .sym = { ._unused = c } } };
                }
                //char* cpy = arena_alloc(l->str_alloc, sizeof(char*), alignof(char*));
                str_slice_t str = { &l->code[begin_cursor], l->cursor - begin_cursor };
                int kw = get_keyword(str);
                if(kw != NOT_A_KEYWORD) {
                    kind = TOKEN_KIND_KEYWORD;
                    return (token_t){ kind, begin_row, begin_col, flags, { .keyword = { .kw = kw } } }; 
                }
                return (token_t){ kind, begin_row, begin_col, flags, { .ident = { str.str, str.len } } };
        }
    }
    return (token_t){ kind, begin_row, begin_col, flags, { .sym = { ._unused = c } } };

ret_symbol:
    l->cursor++;
    l->col++;
    return (token_t){ kind, begin_row, begin_col, flags, { .sym = { ._unused = c } } };
}
