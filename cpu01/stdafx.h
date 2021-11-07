#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_

//
// Included Files
//
#include "F28x_Project.h"

typedef union {
    float *ptr; //Aligned to lower 16-bits
    Uint32 pad; //32-bits
}CLA_FPTR;

typedef union {
    struct trigonometric_struct *ptr; //Aligned to lower 16-bits
    Uint32 pad; //32-bits
}CLA_TRIGPTR;

#include "F2837xD_Cla_defines.h"
#include "CLA_math/CLAmath.h"
#include <stdint.h>

#include <Controllers.h>
#include <PLL.h>
#include <Inverter.h>
#include "CPU_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

struct Timer_PWM_struct
{
    Uint16 CLA_START_TASK;
    Uint16 CLA_END_TASK;
    Uint16 CPU_START_HWI;
    Uint16 CPU_END_HWI;
};

extern struct Timer_PWM_struct Timer_PWM;

#define TIMESTAMP_PWM EPwm10Regs.TBCTR

union ALARM
{
    Uint32 all[2];
    struct
    {
        Uint32 U_dc_0_L:1;
        Uint32 U_dc_0_H:1;
        Uint32 U_dc_1_L:1;
        Uint32 U_dc_1_H:1;
        Uint32 U_dc_2_L:1;
        Uint32 U_dc_2_H:1;
        Uint32 U_dc_3_L:1;
        Uint32 U_dc_3_H:1;

        Uint32 I_conv_0_L:1;
        Uint32 I_conv_0_H:1;
        Uint32 I_conv_1_L:1;
        Uint32 I_conv_1_H:1;
        Uint32 I_conv_2_L:1;
        Uint32 I_conv_2_H:1;
        Uint32 I_conv_3_L:1;
        Uint32 I_conv_3_H:1;

        Uint32 I_inv_0_L:1;
        Uint32 I_inv_0_H:1;
        Uint32 I_inv_1_L:1;
        Uint32 I_inv_1_H:1;
        Uint32 I_inv_2_L:1;
        Uint32 I_inv_2_H:1;
        Uint32 I_inv_3_L:1;
        Uint32 I_inv_3_H:1;

        Uint32 temperature_0A_H:1;
        Uint32 temperature_0B_H:1;
        Uint32 temperature_1A_H:1;
        Uint32 temperature_1B_H:1;
        Uint32 temperature_2A_H:1;
        Uint32 temperature_2B_H:1;
        Uint32 temperature_3A_H:1;
        Uint32 temperature_3B_H:1;

        Uint32 I_grid_L:1;
        Uint32 I_grid_H:1;

        Uint32 Not_enough_data : 1;

        Uint32 rsvd1:29;
    }bit;
};

union STATUS
{
    Uint32 all[2];
    struct
    {
        Uint32 Init_done:1;
        Uint32 SD_card_not_enough_data:1;
        Uint32 SD_no_calibration : 1;
        Uint32 SD_no_settings : 1;
        Uint32 rsvd1 : 28;
        Uint32 rsvd2 : 32;
    }bit;
};

union CONTROL
{
    Uint32 all[16];
    struct
    {
        struct control_bits_struct
        {
            Uint32 SD_save_settings:1;
            Uint32 CPU_reset:1;
            Uint32 rsvd1:30;
        }control_bits;
    }fields;
};

struct Measurements_gain_offset_struct
{
    float temperature;
    float I_conv_0;
    float U_dc_0;
    float I_conv_1;
    float U_dc_1;
    float I_conv_2;
    float U_dc_2;
    float I_conv_3;
    float U_dc_3;
    float I_grid;
    float U_grid;
    float I_inv_0;
    float I_inv_1;
    float I_inv_2;
    float I_inv_3;
};

struct Measurements_alarm_struct
{
    float U_dc;
    float I_conv;
    float I_grid;
    float temperature;
    float I_inv;
};

union EMIF_union
{
    struct
    {
        int16 temperature_0A;
        int16 temperature_0B;
        int16 temperature_1A;
        int16 temperature_1B;
        int16 padding1[12];
        int16 temperature_2A;
        int16 temperature_2B;
        int16 temperature_3A;
        int16 temperature_3B;
        int16 padding2[12];
        int16 U_dc_0;
        int16 I_conv_0;
        int16 U_dc_1;
        int16 I_conv_1;
        int16 padding3[12];
        int16 U_dc_2;
        int16 I_conv_2;
        int16 U_dc_3;
        int16 I_conv_3;
        int16 padding4[12];
        int16 I_grid;
        int16 U_grid;
        int16 I_inv_0;
        int16 I_inv_1;
        int16 padding5[12];
        int16 I_inv_2;
        int16 I_inv_3;

    }read;

    struct
    {
        Uint16 dummy;
    }write;
};

struct EMIF_CLA_struct
{
    int16 temperature_0A;
    int16 temperature_0B;
    int16 temperature_1A;
    int16 temperature_1B;
    int16 temperature_2A;
    int16 temperature_2B;
    int16 temperature_3A;
    int16 temperature_3B;
    int16 U_dc_0;
    int16 I_conv_0;
    int16 U_dc_1;
    int16 I_conv_1;
    int16 U_dc_2;
    int16 I_conv_2;
    int16 U_dc_3;
    int16 I_conv_3;
    int16 I_grid;
    int16 U_grid;
    int16 I_inv_0;
    int16 I_inv_1;
    int16 I_inv_2;
    int16 I_inv_3;

};

//
//Task 1 (C) Variables
//
extern struct CPU1toCPU2_struct CPU1toCPU2;
extern struct CPU2toCPU1_struct CPU2toCPU1;
extern struct CLA2toCLA1_struct CLA2toCLA1;

extern struct Measurements_struct Meas;
extern struct Measurements_gain_offset_struct Meas_gain;
extern struct Measurements_gain_offset_struct Meas_offset;
extern struct Measurements_alarm_struct Meas_alarm_H;
extern struct Measurements_alarm_struct Meas_alarm_L;

extern volatile union EMIF_union EMIF_mem;
extern struct EMIF_CLA_struct EMIF_CLA;

extern union STATUS status;
extern union CONTROL control;
extern union ALARM alarm;
extern union ALARM alarm_snapshot;

extern struct CIC2_struct CIC2_calibration;
extern CLA_FPTR CIC2_calibration_input;

extern float Filter1_CLAasm(float input, float filter, float coefficient);
extern void Fast_copy12_CPUasm();
extern void Fast_copy21_CPUasm();

//
//Task 2 (C) Variables
//

//
//Task 3 (C) Variables
//

//
//Task 4 (C) Variables
//

//
//Task 5 (C) Variables
//

//
//Task 6 (C) Variables
//

//
//Task 7 (C) Variables
//

//
//Task 8 (C) Variables
//

//
// Function Prototypes
//
// The following are symbols defined in the CLA assembly code
// Including them in the shared header file makes them
// .global and the main CPU can make use of them.
//
__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

#ifdef __cplusplus
}
#endif // extern "C"

#endif //end of _CLA_DIVIDE_SHARED_H_ definition

//
// End of file
//
