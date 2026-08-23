#include <stddef.h>
#include <inttypes.h>

// sym table maps integer ids to symbols (identifiers)

#define INIT_SYMTBL_CAPACITY 16

typedef struct sym_tbl_entry_t {
    unsigned int key;
    char* val;
} sym_tbl_entry_t;

typedef struct sym_tbl_t { 
    sym_tbl_entry_t* items;
    size_t cap;
    size_t len; 
} sym_tbl_t;

sym_tbl_t* symtbl_new(size_t init_cap);
void symtbl_free(sym_tbl_t* tbl);
char* symtbl_mov(int key, char* val);
char* symtbl_get(int key);
char* symtbl_rm(int key);
size_t symtbl_len(sym_tbl_t* syms);
