#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

static void load_stdin(uint8_t *data, size_t file_size)
{
    int i;
    
    for(i = 0; i < file_size; ++i)
        data[i] = fgetc(stdin);
}

static void write_stdout(uint8_t *data, size_t count)
{
    fwrite(data, sizeof(uint8_t), count, stdout);
}

static size_t file_size(const char *file)
{
    FILE *fp;
    size_t size = 0;
    
    fp = fopen(file, "rb");
    assert(fp);
    
    while(fgetc(fp) != EOF)
        ++size;

    fclose(fp);
    
    return size;
}

static uint8_t *load_file(const char *file, size_t file_size)
{
    FILE *fp;
    uint8_t *data = malloc(file_size);
    assert(data);
    
    fp = fopen(file, "rb");
    assert(fp);
    
    fread(data, sizeof(uint8_t), file_size, fp);
    
    fclose(fp);
    
    return data;
}

static void write_out(uint8_t *data, size_t count, const char *file)
{
    FILE *fp = fopen(file, "wb");
    assert(fp);
    
    fwrite(data, sizeof(uint8_t), count, fp);
    
    fclose(fp);
}

#endif
