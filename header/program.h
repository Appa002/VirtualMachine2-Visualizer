//
// Created by a_mod on 29.05.2018.
//

#ifndef VM_VISUALIZER_PROGRAM_H
#define VM_VISUALIZER_PROGRAM_H

#include <stdint.h>

typedef struct Line{
    uint32_t line_address;
    uint32_t line_raw_size;

    char* instruction_name;
    uint8_t instruction_opt;
    uint32_t* instruction_args;
    uint32_t instruction_args_amount;

} line_t;

typedef struct Program{
    uint32_t raw_size;
    uint32_t allocated_size;
    uint32_t line_amount;
    line_t** lines;
} program_t;

#endif //VM_VISUALIZER_PROGRAM_H
