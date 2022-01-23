
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

    struct PR_struct PR_I_arm0;
    struct PR_struct PR_I_arm1;
    struct PR_struct PR_I_arm2;
    struct PR_struct PR_I_arm3;

    struct PI_struct PI_I_arm_dc0;
    struct PI_struct PI_I_arm_dc1;
    struct PI_struct PI_I_arm_dc2;
    struct PI_struct PI_I_arm_dc3;
    struct PI_struct PI_I_arm_dc0_cross;
    struct PI_struct PI_I_arm_dc1_cross;
    struct PI_struct PI_I_arm_dc2_cross;
    struct PI_struct PI_I_arm_dc3_cross;

    float I_arm_ac_ref;
    float I_arm_dc_ref;

    float I_arm_ref;

    float Cross1;
    float Cross2;
    float Cross3;
    float Cross0;

    float U_ref0_cross;
    float U_ref1_cross;
    float U_ref2_cross;
    float U_ref3_cross;

    float U_ref0;
    float U_ref1;
    float U_ref2;
    float U_ref3;


    float I_arm2_ref;
    float I_arm_filter;
    float I_arm2_filter2;
    float I_arm2_filter_coeff;

    float I_lim;
    float I_lim_nominal;

    float RDY;
    float enable;
    float theta;
    float zero_error[4];
    float zero_error_cross[4];

    float Ts;
    float L_conv;
    float R_conv;

    float Kp_I;

    float duty[8];
    float deadtime;

    float compensation;
    float resonant_odd_number;
    float resonant_even_number;
    struct Resonant_struct Resonant_I_a_odd0[25];
    struct Resonant_struct Resonant_I_a_odd1[25];

    struct Resonant_struct Resonant_I_a_odd2[25];
    struct Resonant_struct Resonant_I_a_odd3[25];

    struct Resonant_struct Resonant_I_a_even0[2];
    struct Resonant_struct Resonant_I_a_even1[2];

    struct Resonant_struct Resonant_I_a_even2[2];
    struct Resonant_struct Resonant_I_a_even3[2];

    struct Resonant_struct Resonant_I_a_odd0_cross[25];
    struct Resonant_struct Resonant_I_a_odd1_cross[25];

    struct Resonant_struct Resonant_I_a_odd2_cross[25];
    struct Resonant_struct Resonant_I_a_odd3_cross[25];

    struct Resonant_struct Resonant_I_a_even0_cross[2];
    struct Resonant_struct Resonant_I_a_even1_cross[2];

    struct Resonant_struct Resonant_I_a_even2_cross[2];
    struct Resonant_struct Resonant_I_a_even3_cross[2];
    CLA_FPTR duty_a;
    CLA_FPTR duty_b;
    float correction;


    //struct CIC_struct I_arm_CIC, U_dc_CIC;

    enum INVERTER_enum state, state_last;
};

extern struct INVERTER_struct INV;

void Inverter_calc();

#endif /* INVERTER_H */
