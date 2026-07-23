#include <stdlib.h>
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
    if(!r)
        return NULL;
    
    r->ptr = r->data;
    r->end = r->data + capacity;
    r->next = NULL;
    return r;
}

// returns NULL if failed to alloc
void* arena_alloc(arena_t* a, size_t size, size_t align) {
    assert(a);
    assert(align & (align-1)); // ensure align is power of 2
    
    if(size <= 0)
        return NULL;
 
    while(true) {
        uintptr_t aligned = ((uintptr_t)a->cur->ptr + (align - 1)) & ~((uintptr_t)(align - 1));
        uint8_t* next = (uint8_t*)(aligned + size);
        if(next <= a->cur->end) {
            a->cur->ptr = next;
            return (void*)aligned;
        } else if(a->cur == a->first) { // try realloc first region to fit element
            // Corner case where if the first element to be allocated
            // is larger than the region size, a new region will be allocated
            // to fit the new element, but the first region will be wasted memory. 
            arena_region_t* r = new_region(aligned + size - 1);
            if(!r)
                return NULL;
            free(a->cur);
            a->cur = a->first = r;
        } else { // alloc new region and retry
            if(!a->cur->next) {
                size_t reg_size = a->def_region_size;
                if(reg_size < aligned + size - 1)
                    reg_size = aligned + size - 1;

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
        free(r);
        r = r->next;
    }
    free(a);
}
