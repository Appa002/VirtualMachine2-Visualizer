#ifndef VIRTUAL_MACHIEN_INSTRUCTIONS_H
#define VIRTUAL_MACHIEN_INSTRUCTIONS_H


#define ADD_INSTRUCTION(name, arg_amount) uint8_t* opt_ ## name (uint8_t* ip, line_t* out) { return general_instruction((ip), (out), #name, arg_amount); }
#define REGISTER_INSTRUCTION(name, opcode) opt[opcode] = opt_ ## name;

#include <stdint.h>
#include <memory.h>

#include "program.h"
#include "utils.h"

typedef uint8_t *(*instruction_t)(uint8_t *ip, line_t *out);

uint8_t *add_to_ip(uint8_t *ip, uint32_t amount) {
    while (amount > 0) {
        ip++;
        if (*ip == ';') {
            ip++;
            while (*ip != ';')
                ip++;
            ip++;
        }
        amount--;
    }
    return ip;
}

uint8_t *general_instruction(uint8_t *ip, line_t *out, const char* name, uint32_t arg_amount) {
    out->instruction_name = create_string(name);
    out->instruction_args_amount = arg_amount;
    out->instruction_args = malloc(sizeof(uint32_t) * arg_amount);
    for (uint32_t i = 0; i < arg_amount * 4; i++){
        uint32_t argument = 0;
        argument |= *(ip + 1) << 24;
        argument |= *(ip + 2) << 16;
        argument |= *(ip + 3) << 8;
        argument |= *(ip + 4);
        out->instruction_args[i] = argument;
    }
    return add_to_ip(ip, 1 + arg_amount * 4);
}

ADD_INSTRUCTION(readRegister0, 0);
ADD_INSTRUCTION(readRegister1, 0);
ADD_INSTRUCTION(readRegister2, 0);
ADD_INSTRUCTION(readRegister3, 0);
ADD_INSTRUCTION(readRegister4, 0);
ADD_INSTRUCTION(readRegister5, 0);
ADD_INSTRUCTION(readRegister6, 0);
ADD_INSTRUCTION(readRegister7, 0);
ADD_INSTRUCTION(readRegister8, 0);
ADD_INSTRUCTION(readRegister9, 0);

ADD_INSTRUCTION(setRegister0, 0);
ADD_INSTRUCTION(setRegister1, 0);
ADD_INSTRUCTION(setRegister2, 0);
ADD_INSTRUCTION(setRegister3, 0);
ADD_INSTRUCTION(setRegister4, 0);
ADD_INSTRUCTION(setRegister5, 0);
ADD_INSTRUCTION(setRegister6, 0);
ADD_INSTRUCTION(setRegister7, 0);
ADD_INSTRUCTION(setRegister8, 0);
ADD_INSTRUCTION(setRegister9, 0);

ADD_INSTRUCTION(setSize, 0)
ADD_INSTRUCTION(move, 0)
ADD_INSTRUCTION(read, 0)
ADD_INSTRUCTION(alloc, 0)

ADD_INSTRUCTION(push, 1)
ADD_INSTRUCTION(remove, 0)

ADD_INSTRUCTION(uadd, 0)
ADD_INSTRUCTION(sadd, 0)
ADD_INSTRUCTION(fadd, 0)

ADD_INSTRUCTION(usub, 0)
ADD_INSTRUCTION(ssub, 0);
ADD_INSTRUCTION(fsub, 0);

ADD_INSTRUCTION(umult, 0);
ADD_INSTRUCTION(smult, 0)
ADD_INSTRUCTION(fmult, 0)

ADD_INSTRUCTION(udiv, 0)
ADD_INSTRUCTION(sdiv, 0)
ADD_INSTRUCTION(fdiv, 0)

ADD_INSTRUCTION(utof, 0)
ADD_INSTRUCTION(stof, 0)

ADD_INSTRUCTION(abs, 0)
ADD_INSTRUCTION(ucmp, 0)
ADD_INSTRUCTION(scmp, 0)
ADD_INSTRUCTION(fcmp, 0)

ADD_INSTRUCTION(jmp, 0)
ADD_INSTRUCTION(jless, 0)
ADD_INSTRUCTION(jgreater, 0)
ADD_INSTRUCTION(jequal, 0)
ADD_INSTRUCTION(jNequal, 0)
ADD_INSTRUCTION(call, 0)
ADD_INSTRUCTION(return, 0)

ADD_INSTRUCTION(int, 0)
ADD_INSTRUCTION(halt, 0)
ADD_INSTRUCTION(noop, 0)
ADD_INSTRUCTION(out, 0)

uint8_t* error(uint8_t *ip, line_t *out){
    out->instruction_name = create_string("<undefined opcode>");
    out->instruction_args_amount = 0;
    return add_to_ip(ip, 1);
}

void register_instructions(instruction_t *opt) {
    for(size_t i = 0; i < 0xf1; i++){
        opt[i] = error;
    }
    REGISTER_INSTRUCTION(readRegister0, 0xa0);
    REGISTER_INSTRUCTION(readRegister1, 0xa1);
    REGISTER_INSTRUCTION(readRegister2, 0xa2);
    REGISTER_INSTRUCTION(readRegister3, 0xa3);
    REGISTER_INSTRUCTION(readRegister4, 0xa4);
    REGISTER_INSTRUCTION(readRegister5, 0xa5);
    REGISTER_INSTRUCTION(readRegister6, 0xa6);
    REGISTER_INSTRUCTION(readRegister7, 0xa7);
    REGISTER_INSTRUCTION(readRegister8, 0xa8);
    REGISTER_INSTRUCTION(readRegister9, 0xa9);

    REGISTER_INSTRUCTION(setRegister0, 0xb0);
    REGISTER_INSTRUCTION(setRegister1, 0xb1);
    REGISTER_INSTRUCTION(setRegister2, 0xb2);
    REGISTER_INSTRUCTION(setRegister3, 0xb3);
    REGISTER_INSTRUCTION(setRegister4, 0xb4);
    REGISTER_INSTRUCTION(setRegister5, 0xb5);
    REGISTER_INSTRUCTION(setRegister6, 0xb6);
    REGISTER_INSTRUCTION(setRegister7, 0xb7);
    REGISTER_INSTRUCTION(setRegister8, 0xb8);
    REGISTER_INSTRUCTION(setRegister9, 0xb9);

    REGISTER_INSTRUCTION(setSize, 0xc0);
    REGISTER_INSTRUCTION(move, 0xc1);
    REGISTER_INSTRUCTION(read, 0xc2);
    REGISTER_INSTRUCTION(alloc, 0xc3)

    REGISTER_INSTRUCTION(push, 0xd0);
    REGISTER_INSTRUCTION(remove, 0xd1);

    REGISTER_INSTRUCTION(uadd, 0xe0);
    REGISTER_INSTRUCTION(sadd, 0xe1);
    REGISTER_INSTRUCTION(fadd, 0xe2);
    REGISTER_INSTRUCTION(usub, 0xe3);
    REGISTER_INSTRUCTION(ssub, 0xe4);
    REGISTER_INSTRUCTION(fsub, 0xe5);
    REGISTER_INSTRUCTION(umult, 0xe6);
    REGISTER_INSTRUCTION(smult, 0xe7);
    REGISTER_INSTRUCTION(fmult, 0xe8);
    REGISTER_INSTRUCTION(udiv, 0xe9);
    REGISTER_INSTRUCTION(sdiv, 0xea);
    REGISTER_INSTRUCTION(fdiv, 0xeb);
    REGISTER_INSTRUCTION(utof, 0xec);
    REGISTER_INSTRUCTION(stof, 0xed);
    REGISTER_INSTRUCTION(abs, 0xee);
    REGISTER_INSTRUCTION(ucmp, 0xef);
    REGISTER_INSTRUCTION(scmp, 0xf0);
    REGISTER_INSTRUCTION(fcmp, 0xf1);

    REGISTER_INSTRUCTION(jmp, 0x01);
    REGISTER_INSTRUCTION(jless, 0x02);
    REGISTER_INSTRUCTION(jgreater, 0x03);
    REGISTER_INSTRUCTION(jequal, 0x04);
    REGISTER_INSTRUCTION(jNequal, 0x05);
    REGISTER_INSTRUCTION(call, 0x06);
    REGISTER_INSTRUCTION(return, 0x07);
    REGISTER_INSTRUCTION(int, 0x10);
    REGISTER_INSTRUCTION(halt, 0x11);
    REGISTER_INSTRUCTION(noop, 0x12)
    REGISTER_INSTRUCTION(out, 0x13)
}

#endif //VIRTUAL_MACHIEN_INSTRUCTIONS_H
