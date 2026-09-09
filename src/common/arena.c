// Copyright 2026 Amirali Yaghoubi
// SPDX-License-Identifier: Apache-2.0
#ifndef ARENA_MIN_BLOCK_SIZE
#define ARENA_MIN_BLOCK_SIZE (32 * 1024)
#endif

#include "common/arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>



static inline ArenaBlock* create_new_arena_block(size_t size)
{
    ArenaBlock* block = malloc(sizeof(ArenaBlock));
    if (!block)
        return NULL;

    block->memory = malloc(size);
    if (!block->memory)
    {
        free(block);
        return NULL;
    }

    block->capacity = size;
    block->offset = 0;
    block->next = NULL;

    return block;
}


inline bool arena_init(Arena* a)
{
    ArenaBlock* block = create_new_arena_block(ARENA_MIN_BLOCK_SIZE);
    if (!block)
        return false;

    a->head = block;
    a->current = a->head;

    return true;
}


inline void* arena_alloc(Arena* a, size_t size)
{
    //aling to 8 bytes
    size = (size + 7) & ~((size_t)7);

    if (a->current->offset + size > a->current->capacity)
    {
        size_t block_size = (size > ARENA_MIN_BLOCK_SIZE) ? size : ARENA_MIN_BLOCK_SIZE;
        ArenaBlock* block = create_new_arena_block(block_size);
        if (!block)
        {
            fprintf(stderr, "Error in allocating memory\n");
            exit(1);
            return NULL;
        }
        a->current->next = block;
        a->current = block;
    }

    void* ptr = a->current->memory + a->current->offset;
    a->current->offset += size;

    return ptr;

}


inline void arena_free(Arena* a)
{
    ArenaBlock *block;
    ArenaBlock *next;
    
    block = a->head;
    while (block)
    {
        next = block->next;
        free(block->memory);
        free(block);
        block = next;
    }
    a->head = NULL;
    a->current = NULL;
}
