#include "include/chunk.h"
#include "include/vm.h"
#include "include/utils.h"

#include <stdio.h>
#include <assert.h>
#include <pigeon/alias.h>
#include <stdio.h>
#include <string.h>

// I'll do the heavy lifting. You just have to make sure to 
// free the memory I allocate ;)
char* slurp_file(const char* path) {
    FILE* file = NULL;
    file = fopen(path, "rb");
    assert(file);

    fseek(file, 0, SEEK_END);
    usize file_size = ftell(file);
    rewind(file);

    char* buffer = (char*) calloc(file_size+1, 1);

    fread(buffer, 1, file_size, file);
    fclose(file);

    return buffer;
}

Chunk* wasm_line_to_bytecode(char* line) {
    Chunk* chunk = chunk_new();

    puts(line);

    return chunk;
}

Chunk* wasm_str_to_bytecode(char* str) {
    Chunk* chunk =  chunk_new();

    char* line = strtok(str, "\n");
    while (line) {
        Chunk* line_chunk = wasm_line_to_bytecode(line);
        chunk_append_chunk(chunk, line_chunk);
        line = strtok(NULL, "\n");
    }

    return chunk;
}

i32 main(void) {
    char* buf = slurp_file("example.asm");

    Chunk* chunk = wasm_str_to_bytecode(buf);
    chunk_hex_dump(chunk);

    // vm_interpret_chunk(chunk);
    // vm_register_dump();

    chunk_free(chunk);
    free(buf);
    return 0;
}
