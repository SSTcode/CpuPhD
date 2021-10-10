/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <stdafx.h>
#include "HWIs.h"
#include <Scope.h>
#include <State.h>

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void HWI_func()
{
    Timer_PWM.CPU_START_HWI = TIMESTAMP_PWM;

    Fast_copy12_CPUasm();

    if(!status.bit.Init_done);
    else
    {
        if(Meas.U_dc < Meas_alarm_L.U_dc) alarm.bit.U_dc_L = 1;
        if(Meas.U_dc > Meas_alarm_H.U_dc) alarm.bit.U_dc_H = 1;

        if((alarm.all[0] | alarm.all[1]) && !(alarm_snapshot.all[0] | alarm_snapshot.all[1]))
        {
            alarm_snapshot = alarm;
        }
    }

    {
        static Uint16 first = 0;
        static volatile Uint16 decimation_ratio = 1;
        static Uint16 decimation = 0;

        if(!first)
        {
            first = 1;

            Scope.acquire_before_trigger = 800;

            Scope.data_in[0] = &Meas.U_dc;
            Scope.acquire_counter = -1;
        }

        if (++decimation >= decimation_ratio)
        {
//            static volatile float *trigger_pointer = &Conv.RDY;
//            static volatile float trigger_val = 0.5f;
//            static volatile Uint16 edge = 1;
//            static volatile float trigger_last = 0;
//            Scope_trigger(*(float *)trigger_pointer, (float *)&trigger_last, trigger_val, 1);
    //        Scope_trigger((float)Comp.state, 1.5f, 1);
    //        Scope_trigger(*trigger_pointer, -trigger_val, 1);
    //        if(VoltDip.Urms_grid.a_counter) Scope_trigger_unc();
    //        if(Machine.state == Machine_Lgrid_meas) Scope_trigger_unc();
            if(alarm.all[0] | alarm.all[1]) Scope_trigger_unc();
            Scope_task();
            decimation = 0;
        }
    }

    Timer_PWM.CPU_END_HWI = TIMESTAMP_PWM;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
