#include <stdio.h>
#include "common/arena.h"
#include "common/file.h"


char* read_file(Arena* a, const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size_temp = ftell(file);
    if (size_temp < 0)
    {
        fclose(file);
        fprintf(stderr, "Error in reading %s\n", path);
        return NULL;
    }
    size_t size = (size_t)size_temp;
    rewind(file);

    char* buffer = arena_alloc(a, size + 1);
    if (!buffer)
    {
        fclose(file);
        fprintf(stderr, "Error in allocating memory from arena\n");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size)
    {
        if (feof(file)) {
            fprintf(stderr, "Warning: Unexpected end of file.\n");
        } else if (ferror(file)) {
            perror("Error reading file");
        }
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}