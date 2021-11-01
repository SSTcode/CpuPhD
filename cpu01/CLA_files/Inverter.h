
#include "stdafx.h"


#ifndef INVERTER
#define INVERTER_H_


enum INVERTER_enum
{
    INV_first,
    INV_active
};

struct INVERTER_struct
{
    struct PR_struct PR_I_arm;
    float I_arm_ref;
    float I_arm2_ref;
    float I_arm_filter;
    float I_arm2_filter2;
    float I_arm2_filter_coeff;

    float RDY;

    float Ts;
    float Lgrid;
    float Rgrid;
    float counter;


    float duty[2];
    float deadtime;

    enum INVERTER_enum state, state_last;
};

extern struct INVERTER_struct INV;

void Inverter_calc();

#endif /* INVERTER_H */
