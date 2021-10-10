/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <stdafx.h>
#include <math.h>
#include "HWIs.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void HWI_func()
{
    Timer_PWM.CPU_START_HWI = TIMESTAMP_PWM;

    Fast_copy12_CPUasm();


    Fast_copy21_CPUasm();

    Timer_PWM.CPU_END_HWI = TIMESTAMP_PWM;

    PieCtrlRegs.PIEACK.bit.ACK5 = 1;
}
