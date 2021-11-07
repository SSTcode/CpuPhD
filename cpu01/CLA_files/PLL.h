#pragma once

#ifndef PLL_H_
#define PLL_H_
#include "Controllers.h"

enum PLL_state_enum
{
    PLL_omega_init,
    PLL_first,
    PLL_check,
    PLL_active
};

struct PLL_struct {
    struct SOGI_struct SOGI;
    struct PI_struct PI;
    float Ts;
    float RDY;
    float omega_nominal;

    float theta;
    float omega;
    float amplitude;
    float Umod_pos;

    float w;
    float w_filter1;
    float w_filter2;
    float w_filter_coeff;

    enum PLL_state_enum state, state_last;
};

extern struct PLL_struct PLL;

void PLL_calc();

#endif /* PLL_H_ */
