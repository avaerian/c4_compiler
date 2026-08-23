#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "arena.h"
#include "lex.h"

const char* TEST_FILE = "../res/example1.c4";

const char* src_file_to_str(FILE* f) { 
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(len + 1);
    fread(buf, sizeof *buf, len, f);
    buf[len] = '\0';
    return buf;
}

int main(int argc, char** argv) {
    printf("sizeof(token_t) = %lu\n", sizeof(token_t));

    const char* file_path = argc > 1 ? argv[1] : TEST_FILE; 
    FILE* f = fopen(file_path, "r"); 
    if(!f) {
        fprintf(stderr, "Failed to open file %s\n", file_path);
        goto cleanup1;
    }
    
    const char* src = src_file_to_str(f);
    arena_t* str_alloc = arena_new(1024);
    if(!str_alloc) {
        fprintf(stderr, "Failed to create string arena allocator");
        goto cleanup2;
    }

    lexer_t* l = lexer_new(src, str_alloc);
    if(!l) {
        fprintf(stderr, "Failed to create lexer");
        goto cleanup3;
    }
    
    // DEBUG
    token_t tok;
    while((tok = lexer_next_token(l)).kind != TOKEN_KIND_EOF)
        printf("Token(row:%d, col:%d, flags: %hu): %s\n", tok.row, tok.col, tok.flags, lexer_token_str(tok, str_alloc));

    printf("Token(row:%d, col:%d): %s\n", tok.row, tok.col, lexer_token_str(tok, str_alloc));

    goto cleanup4;

cleanup4: lexer_free(l);
cleanup3: arena_free(str_alloc);
cleanup2: free(f);
cleanup1: exit(1);
}
