//
// Created by a_mod on 02.06.2018.
//

#ifndef VM_VISUALIZER_IO_H
#define VM_VISUALIZER_IO_H

#include <stdlib.h>
#include <stdio.h>
#include "program.h"

typedef struct AddableRawData{
    uint8_t* data;
    size_t pos;
} addable_data_t;

addable_data_t createAddableData (size_t size){
    addable_data_t out;
    out.data = malloc(sizeof(uint8_t) * size);
    out.pos = 0;
    return out;
};

void addByte(addable_data_t* input, uint8_t value){
    input->data[input->pos++] = value;
}

int load_file(const char *filename, unsigned char **result, size_t* size) {
    *size = 0;
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        *result = NULL;
        return -1;
    }
    fseek(f, 0, SEEK_END);
    *size = (unsigned int)ftell(f);
    fseek(f, 0, SEEK_SET);
    *result = (unsigned char *)malloc(*size+1);
    if (*size != fread(*result, sizeof(char), *size, f))
    {
        free(*result);
        return -2; // -2 means file reading fail
    }
    fclose(f);
    (*result)[*size] = 0;
    return 0;
}

int save_file(const char *filename, program_t *program){
    FILE* f = fopen(filename, "w");
    if(f == NULL)
        return -1;

    addable_data_t data  = createAddableData(program->raw_size);
    line_t *line = NULL;

    for(size_t i = 0; i < program->line_amount; i++){
        line = program->lines[i];
        addByte(&data, program->lines[i]->instruction_opt);

        for(size_t j = 0; j < line->instruction_args_amount; j++){
            for(size_t k = 1; k < 4 + 1; k++){
                uint8_t argumentPart = (uint8_t) ((
                        line->instruction_args[j]
                        << 8 * (k - 1))
                        >> 8 * (k - 1)
                        >> 32 - 8 * k);

                addByte(&data, argumentPart);
            }
        }
    }

    if(fwrite(data.data, sizeof(uint8_t), program->raw_size, f) < program->raw_size){
        free(data.data);
        return  -2;
    }

    fclose(f);
    return 0;
}
#endif //VM_VISUALIZER_IO_H
