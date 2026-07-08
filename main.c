#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
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
    const char* file_path = argc > 1 ? argv[1] : TEST_FILE; 
    FILE* f = fopen(file_path, "r"); 
    if(!f) {
        printf("Failed to open file %s\n", file_path);
        goto cleanup1;
    }
    
    const char* src = src_file_to_str(f);
    lexer_t* l = lexer_new(src);
    if(!l) {
        printf("Failed to create lexer");
        goto cleanup2;
    }
    
    // DEBUG
    token_t tok;
    while((tok = lexer_new_token(l)).kind != TOKEN_KIND_EOF)
        printf("Token(row:%d, col:%d): %s\n", tok.row, tok.col, lexer_token_str(tok));
    
    goto cleanup3;

cleanup3: free(l);
cleanup2: free(f);
cleanup1: exit(1);
}
