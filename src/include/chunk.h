#ifndef __CHUNK_H
#define __CHUNK_H

#include <pigeon/alias.h>

typedef struct chunk_t {
    u8* at; // The raw bytes
    u16 len; // Length of data
    u16 capacity; // Capacity of data
} Chunk; // Dynamic Array of Bytes

Chunk* chunk_new();
void chunk_free(Chunk* chunk);
void chunk_append_byte(Chunk* chunk, u8 byte);
void __chunk_append_bytes(Chunk* chunk, u8* bytes, u16 bytes_len);
#define chunk_append_bytes(chunk, bytes_arr) \
    __chunk_append_bytes((chunk), (bytes_arr), LEN((bytes_arr)))
void chunk_append_chunk(Chunk* chunk, Chunk* appendee);
void chunk_hex_dump(const Chunk* chunk);

#endif  //__CHUNK_H
