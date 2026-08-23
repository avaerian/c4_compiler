#include <stdlib.h>
#include <stdio.h>
#include "sym_tbl.h"

//TODO: ensure init_cap is power of 2
sym_tbl_t* symtbl_new(size_t init_cap) {
    init_cap = (init_cap >> 1) << 1;
    printf("init_cap: %lu\n", init_cap);
    if(init_cap <= 0) {
        return NULL;
    }

    sym_tbl_t* tbl = malloc(sizeof(sym_tbl_t));
    if(!tbl) {
        return NULL;
    }

    tbl->items = malloc(sizeof(sym_tbl_entry_t) * init_cap);
    tbl->cap = init_cap;
    tbl->len = 0;
    return tbl;
}

void symtbl_free(sym_tbl_t* tbl) {
    free(tbl->items);
    free(tbl);
}

char* symtbl_mov(int key, char* val) {
    return NULL;
}

char* symtbl_get(int key) {
    return NULL;
}

char* symtbl_rm(int key) {
    return NULL;
}

size_t symtbl_len(sym_tbl_t* syms) {
    return syms->len;
}
