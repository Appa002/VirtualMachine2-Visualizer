//
// Created by a_mod on 13.07.2018.
//

#include <stdint.h>
#include <math.h>

#ifndef VM_VISUALIZER_MATHS_H
#define VM_VISUALIZER_MATHS_H

typedef struct FloatParts{
        uint32_t sign;
        uint32_t exponent;
        uint32_t mantissa;
} float_part_t;

float_part_t splitIEEE754Float(uint32_t input){
    float_part_t out;
    out.sign = input >> 31;
    out.exponent = (input << 1) >> 24;
    out.mantissa = (input << 9) >> 9;
    return out;
}


uint32_t readNthBit(uint32_t value, uint32_t n){
    value = value >> n;
    value = value & (uint32_t)1;
    return value;
}


float readIEEE754Float(uint32_t input){
    float_part_t parts = splitIEEE754Float(input);
    const uint32_t bias = 127;

    /* Calculates the fraction given by the mantissa.
     * */
    double fraction = 0;
    for(uint32_t i = 0; i < 23; i++){
        fraction += readNthBit(input, 22 - i) / pow(2, i + 1);
    }
    /*LaTex of below equation:
     * \( value=(-1)^{sign}*(fraction + 1)*2^{exponent - bias} \)
     * where: sign is the signage of the float
     *        mantissa is the mantissa of the float
     *        exponent is the exponent of the float
     *        bias is a constant, here for single-precision float 127*/

    double value = pow(-1, parts.sign) * (1 + fraction) * pow(2, (int32_t)parts.exponent - (int32_t)bias);


    return (float)value;
}

#endif //VM_VISUALIZER_MATHS_H
