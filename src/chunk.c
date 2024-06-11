#include "include/chunk.h"

#include <assert.h>
#include <stdlib.h>

#define CHUNK_MAX (256 * 256)
#define CHUNK_DEFAULT_LEN (16)

Chunk* chunk_new() {
    Chunk* chunk = calloc(1, sizeof(Chunk));
    assert(chunk);
    chunk->at = calloc(1, sizeof(u8) * CHUNK_DEFAULT_LEN);
    assert(chunk->at);
    chunk->capacity = CHUNK_DEFAULT_LEN;
    return chunk;
}

void chunk_free(Chunk* chunk) {
    assert(chunk);
    free(chunk->at); 
    free(chunk);
}

void chunk_hex_dump(const Chunk* chunk) {
    for (u16 i = 0; i < chunk->len; ++i) {
        if ((i) % 4 == 0) {
            printf("0x%.4X    ", i);
        }

        printf("%.2X", chunk->at[i]);
        if (i + 1 < chunk->len) {
            printf(" ");

            if ((i - 3) % 4 == 0) {
                putchar('\n');
            }
        }

    }

    putchar('\n');
}

static void chunk_resize(Chunk* chunk) {
    chunk->capacity *= 2;
    chunk->at = realloc(chunk->at, chunk->capacity);
    assert(chunk->at);
}

void chunk_append_byte(Chunk* chunk, u8 byte) {
    if (chunk->len == chunk->capacity) {
        chunk_resize(chunk);
    }

    chunk->at[chunk->len++] = byte;
}

void __chunk_append_bytes(Chunk* chunk, u8* bytes, u16 bytes_len) {
    for (u16 i = 0; i < bytes_len; ++i) {
        chunk_append_byte(chunk, bytes[i]);
    }
}

// Discards the appended chunk as well
void chunk_append_chunk(Chunk* chunk, Chunk* appendee) {
    while (appendee->len > 0) {
        chunk_append_byte(chunk, appendee->at[--appendee->len]);
    }
    chunk_free(appendee);
}
