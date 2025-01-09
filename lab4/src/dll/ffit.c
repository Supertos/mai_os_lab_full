#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

typedef struct Allocator {
    void *memory;
    size_t size;
    FreeBlock *free_list;
} Allocator;

Allocator* allocator_create(void *const memory, const size_t size) {
    Allocator *allocator = (Allocator *)memory;
    allocator->size = size;
    allocator->free_list = (FreeBlock *)((char *)memory + sizeof(Allocator));
    allocator->free_list->size = size - sizeof(Allocator);
    allocator->free_list->next = NULL;
    return allocator;
}

void allocator_destroy(Allocator *const allocator) {
}

void* allocator_alloc(Allocator *const allocator, const size_t size) {
    FreeBlock *prev = NULL;
    FreeBlock *curr = allocator->free_list;

    while (curr != NULL) {
        if (curr->size >= size) {
            if (curr->size > size + sizeof(FreeBlock)) {
                FreeBlock *new_block = (FreeBlock *)((char *)curr + size);
                new_block->size = curr->size - size;
                new_block->next = curr->next;
                curr->size = size;
                curr->next = new_block;
            }

            if (prev == NULL) {
                allocator->free_list = curr->next;
            } else {
                prev->next = curr->next;
            }

            return (void *)((char *)curr + sizeof(FreeBlock));
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL;
}

void allocator_free(Allocator *const allocator, void *const memory) {
    FreeBlock *block = (FreeBlock *)((char *)memory - sizeof(FreeBlock));
    block->next = allocator->free_list;
    allocator->free_list = block;
}

