
#include "stdafx.h"


#ifndef INVERTER
#define INVERTER_H_


enum INVERTER_enum
{
    INV_first,
    INV_active,
    __dummybig_INV = 300000
};

struct INVERTER_struct
{
    struct CIC1_struct CIC1_I_arm;
    struct Resonant_struct Resonant_U_grid;
    struct Resonant_struct Resonant_I_grid;

    struct PR_struct PR_I_arm;
    struct PR_struct PR_I_arm2;
    struct PR_struct PR_I_arm_cross;
    struct PR_struct PR_I_arm2_cross;
    struct PI_struct PI_I_arm_dc;
    struct PI_struct PI_I_arm2_dc;
    struct PI_struct PI_I_arm_dc_cross;
    struct PI_struct PI_I_arm2_dc_cross;
    float I_arm_ref;
    float I_arm_dc_ref;

    float I_cross_0;
    float I_cross_1;

    float I_lim;
    float I_lim_nominal;

    float RDY;
    float RDY_cross;
    float enable;
    float theta;
    float zero_error;

    float Ts;
    float L_conv;
    float R_conv;

    float Kp_I;

    float duty[8];
    float deadtime;

    float compensation;
    float resonant_odd_number;
    float resonant_even_number;
    struct Resonant_struct Resonant_I_a_odd[25];
    struct Resonant_struct Resonant_I_b_odd[25];

    struct Resonant_struct Resonant_I_a_even[2];
    struct Resonant_struct Resonant_I_b_even[2];
    CLA_FPTR duty_a;
    CLA_FPTR duty_b;
    float correction;

    //struct CIC_struct I_arm_CIC, U_dc_CIC;

    enum INVERTER_enum state, state_last;
};

extern struct INVERTER_struct INV;

void Inverter_calc();

#endif /* INVERTER_H */
