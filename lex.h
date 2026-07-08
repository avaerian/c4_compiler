#include <inttypes.h>

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

typedef struct lexer {
    const char* code;
    uint32_t cursor;
    uint32_t line;
    uint32_t col;
} lexer_t;

typedef struct token_sym {
    //sym_kind_t sym;
    token_kind_t _unused; //FIXME
} token_sym_t;

typedef struct token_comment {
    const char* comment;
    uint32_t len;
} token_comment_t;

typedef struct token_ident {
    const char* ident;
} token_ident_t;

typedef struct token {
    token_kind_t kind; // type to identify union
    union {
        token_sym_t sym;
        token_ident_t ident;
        token_comment_t comment;
    } data; 
    uint32_t row, col;
} token_t;

lexer_t* lexer_new(const char* src);
token_t lexer_new_token(lexer_t* l);
const char* lexer_token_str(token_t t);
void lexer_free(lexer_t* l);
