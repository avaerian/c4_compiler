#ifndef _LEX_H
#define _LEX_H

#include <inttypes.h>
#include "arena.h"

typedef unsigned char token_kind_t; 

extern const token_kind_t TOKEN_KIND_EOF;
extern const token_kind_t TOKEN_KIND_IDENT;
extern const token_kind_t TOKEN_KIND_LOGICAL_AND; // &&
extern const token_kind_t TOKEN_KIND_LOGICAL_OR; // ||
extern const token_kind_t TOKEN_KIND_R_ARROW; // ->
extern const token_kind_t TOKEN_KIND_L_ARROW; // <-
extern const token_kind_t TOKEN_KIND_COMMENT; //
extern const token_kind_t TOKEN_KIND_L_BITSHIFT; // <<
extern const token_kind_t TOKEN_KIND_R_BITSHIFT; // >>
extern const token_kind_t TOKEN_KIND_NE;
extern const token_kind_t TOKEN_KIND_EQ; // ==
extern const token_kind_t TOKEN_KIND_LTE; // <=
extern const token_kind_t TOKEN_KIND_GTE; // >=
extern const token_kind_t TOKEN_KIND_INCR; // ++
extern const token_kind_t TOKEN_KIND_DECR; // --
extern const token_kind_t TOKEN_KIND_ADD_ASSIGN; //+=
extern const token_kind_t TOKEN_KIND_SUB_ASSIGN; //-=
extern const token_kind_t TOKEN_KIND_MUL_ASSIGN; //*=
extern const token_kind_t TOKEN_KIND_DIV_ASSIGN; ///=
extern const token_kind_t TOKEN_KIND_MOD_ASSIGN; //%= 
extern const token_kind_t TOKEN_KIND_AND_ASSIGN; //&=
extern const token_kind_t TOKEN_KIND_OR_ASSIGN;  //|=
extern const token_kind_t TOKEN_KIND_KEYWORD;

typedef struct {
    char* code;
    uint32_t cursor;
    uint32_t line;
    uint32_t col;
    arena_t* str_alloc;
} lexer_t;

typedef enum {
    FOR = 0, 
    WHILE,
    IF,
    ELSE,
    FN,
    STRUCT,
    ENUM,
    PUB,
    MUT,
    RETURN,
    
    CHAR, //remove and instead allow u8 to support chars and ints as values
    U0, U8, U16, U32, U64, USIZE,
    I8, I16, I32, I64, ISIZE,
    F32, F64,

    VOID,
    EXTERN,
    CONST,
    STATIC,
    UNION,
    ASSERT,
    USE,
    IMPORT,

    SWITCH,
    CASE,
    CONTINUE,
    BREAK,

    KEYWORD_LEN // not a keyword; used purely for retrieving size
} kw_t;

typedef struct {
    token_kind_t kind;
    union {
        struct {
            char _unused;
        } sym;

        struct {
            char* ident; //TODO: use symbol table instead of copying each ident
            uint32_t len;
        } ident;

        struct {
            enum {
                SINGLE_LINE, //
                MULTI_LINE, /* */
                DOCS, /** */                //TODO: review
            } type;
            char* comment;
            uint32_t len;
        } comment;
        
        struct {
            kw_t kw;
        } keyword;
    } data;
    uint32_t row, col;
} token_t;

lexer_t* lexer_new(char* src, arena_t* str_alloc);
token_t lexer_new_token(lexer_t* l);
inline const char* kw_str(kw_t kw);
const char* lexer_token_str(token_t t);
void lexer_free(lexer_t* l);

#endif // _LEX_H
