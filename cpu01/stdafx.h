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

#include "Controllers.h"
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
        Uint32 U_dc_H:1;
        Uint32 U_dc_L:1;
        Uint32 Not_enough_data : 1;

        Uint32 rsvd1:29;
        Uint32 rsvd2:32;
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
    float U_dc;
};

struct Measurements_alarm_struct
{
    float U_dc;
    float Temp;
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