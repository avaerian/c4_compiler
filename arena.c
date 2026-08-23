#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "arena.h"

extern arena_region_t* new_region(size_t len);

arena_t* arena_new(size_t def_region_size) {
    arena_t* a = malloc(sizeof(arena_t));
    if(!a)
        goto exit;

    a->cur = a->first = new_region(def_region_size);
    if(!a->cur)
        goto cleanup1;

    a->def_region_size = def_region_size;

    return a;

cleanup1: free(a);
exit: return NULL;
}

arena_region_t* new_region(size_t capacity) {
    size_t len = sizeof(arena_region_t) + capacity;
    arena_region_t* r = malloc(len);
    if(!r) {
        fprintf(stderr, "Failed to malloc new arena_region_t");
        return NULL;
    }
    
    r->ptr = r->data;
    r->end = r->data + capacity;
    r->next = NULL;
    return r;
}

// returns NULL if failed to alloc
void* arena_alloc(arena_t* a, size_t size) {
    assert(a);
    
    if(size <= 0)
        return NULL;
 
    while(true) {
        uint8_t* next = a->cur->ptr + size; 
        if(next <= a->cur->end) {
            uint8_t* ptr = a->cur->ptr;
            a->cur->ptr = next;
            return (void*)ptr;
        } else if(a->cur == a->first && a->cur->ptr == a->cur->data) { // try realloc first region to fit element
            // Corner case where if the first element to be allocated
            // is larger than the region size, a new region will be allocated
            // to fit the new element, but the first region will be wasted memory. 
            arena_region_t* r = new_region(size);
            if(!r)
                return NULL;

            r->next = a->cur; //set initial region to next
            a->cur = a->first = r;
        } else { // alloc new region and retry
            if(!a->cur->next) {
                size_t reg_size = a->def_region_size;
                if(reg_size < size)
                    reg_size = size;

                assert(reg_size >= size);

                a->cur->next = new_region(reg_size);
                if(!a->cur->next)
                    return NULL;
            }
            a->cur = a->cur->next;
        }
    }
}

void arena_reset(arena_t* a) {
    assert(a);
    for(arena_region_t* r = a->first; r; r = r->next)
        r->ptr = r->data;
    a->cur = a->first;
}

void arena_free(arena_t* a) {
    assert(a);
    arena_region_t* r = a->first;
    while(r) {
        arena_region_t* next = r->next;
        free(r);
        r = next;
    }
    free(a);
}
