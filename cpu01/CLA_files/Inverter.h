
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
    struct PR_struct PR_I_arm2;
    struct PR_struct PR_I_arm_cross;
    struct PR_struct PR_I_arm2_cross;
    float I_arm_ref;
    float I_arm2_ref;
    float I_arm_filter;
    float I_arm2_filter2;
    float I_arm2_filter_coeff;

    float I_cross_0;
    float I_cross_1;

    float RDY;
    float enable;
    float theta;

    float Ts;
    float L_conv;
    float R_conv;

    float duty[8];
    float deadtime;

    //struct CIC_struct I_arm_CIC, U_dc_CIC;

    enum INVERTER_enum state, state_last;
};

extern struct INVERTER_struct INV;

void Inverter_calc();

#endif /* INVERTER_H */
