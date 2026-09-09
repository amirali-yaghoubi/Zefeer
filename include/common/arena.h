// Copyright 2026 Amirali Yaghoubi
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



typedef struct ArenaBlock{
    uint8_t* memory;
    size_t capacity;
    size_t offset;
    struct ArenaBlock* next;
} ArenaBlock;

typedef struct {
    ArenaBlock *head;
    ArenaBlock *current;
} Arena;



bool arena_init(Arena* a);
void* arena_alloc(Arena* a, size_t size);
void arena_free(Arena* a);
