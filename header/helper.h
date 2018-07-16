//
// Created by a_mod on 29.05.2018.
//

#ifndef VM_VISUALIZER_HELPER_H
#define VM_VISUALIZER_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

#include "program.h"
#include "instructions.h"
#include "io.h"

program_t* generate_program(const char* filename){
    unsigned char* raw_data;
    size_t size;
    if(load_file(filename, &raw_data, &size) != 0)
        exit(3);

    program_t* out = malloc(sizeof(program_t));
    out->raw_size = (uint32_t)size;
    out->lines = (line_t**)malloc(sizeof(line_t*) * (size + 1));
    out->allocated_size = (uint32_t)size + 1;

    uint8_t* ip = raw_data;
    instruction_t* opt = malloc(sizeof(instruction_t) * 0xf2);
    register_instructions(opt);

    if(*ip == 0x00){
        *ip = 0x11;
        out->raw_size++;
    }

    uint32_t i = 0;
    do{
        line_t* line = malloc(sizeof(line_t));
        line->instruction_opt = *ip;
        line->line_address = (uint32_t)(ip - raw_data);
        if(*ip > 0xf1)
            ip = opt[0](ip, line);
        ip = opt[*ip](ip, line);
        line->line_raw_size = 1 + line->instruction_args_amount * 4;
        out->lines[i] = line;
        i++;
    }while((*ip) != raw_data[size] && size != 0);
    out->line_amount = i;
    return out;
}

void delete_program(program_t* program){
    for(uint32_t i = 0; i < program->line_amount; i++){
        if(program->lines[i]->instruction_args_amount != 0)
            free(program->lines[i]->instruction_args);
        free(program->lines[i]->instruction_name);
        free(program->lines[i]);
    }
    free(program);
}

program_t* realocate_program(program_t* program, uint32_t size){
    program_t* out = (program_t*)malloc(sizeof(program_t));
    out->raw_size = program->raw_size;
    out->lines = (line_t**)malloc(sizeof(line_t*) * size);
    out->allocated_size = size;
    out->line_amount = program->line_amount;

    for(uint32_t i = 0; i < program->line_amount; i++){
        out->lines[i] = program->lines[i];
    }
  //  free(program);
    return out;
}

void replace_arg(uint32_t line_index, int element, program_t *program, uint32_t new_val){
    if(element < 2)
        return;
    program->lines[line_index]->instruction_args[element - 2] = new_val;
}

void delete_line(uint32_t line_index, program_t* program){
    program->raw_size -= program->lines[line_index]->line_raw_size;
    program->line_amount--;
    free(program->lines[line_index]);
    for(int i = line_index; i < program->line_amount; i++){
        program->lines[i] = program->lines[i + 1];
    }
    program->lines[0]->line_address = 0;
    for(int i = 1; i < program->line_amount; i++){
        program->lines[i]->line_address =
                program->lines[i - 1]->line_address + program->lines[i - 1]->line_raw_size;
    }
}

void add_line(uint8_t opt_code, uint32_t insert_index, program_t** program){
    uint8_t code[256];
    for(int i = 0; i < 256; i++){
        code[i]= 0;
    }
    code[0] = opt_code;
    instruction_t* opts = malloc(sizeof(instruction_t) * 0xf2);
    register_instructions(opts);

    line_t* line = malloc(sizeof(line_t));
    if(opt_code > 0xf1)
        opts[0](code, line);
    else
        opts[opt_code](code, line);

    line->line_raw_size = 1 + line->instruction_args_amount * 4;
    line->instruction_opt = opt_code;

   if((*program)->allocated_size < (*program)->raw_size + line->line_raw_size){
       *program = realocate_program(*program, (*program)->allocated_size * 2);
   }

   line_t* temp = NULL;
   for(uint32_t i = insert_index; i < (*program)->line_amount + 1; i++){
       line_t* n = (*program)->lines[i];
       (*program)->lines[i] = temp;
       temp = n;

   }

    (*program)->lines[insert_index] = line;
    (*program)->line_amount++;
    (*program)->raw_size += line->line_raw_size;

    if(insert_index == 0)
        (*program)->lines[0]->line_address = 0;

   for(uint32_t i = insert_index; i < (*program)->line_amount; i++){
       if((int)i - 1 >= 0){

           (*program)->lines[i]->line_address =
                   (*program)->lines[i - 1]->line_address + (*program)->lines[i - 1]->line_raw_size;

       }
   }

}

#endif