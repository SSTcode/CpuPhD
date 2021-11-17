/*
 * HWIs.cpp
 * Hardware Interrups
 *
 */

#include <stdafx.h>
#include "HWIs.h"
#include <Scope.h>
#include <State.h>
#include <string.h>

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void HWI_func()
{
    //#TODO change in FPGA TZ_EN pin number to PWM9A

    Timer_PWM.CPU_START_HWI = TIMESTAMP_PWM;
    //memcpy((Uint16 *)&EMIF_CLA, (Uint16 *)&EMIF_mem.read, sizeof(EMIF_CLA));
    Fast_copy12_CPUasm();

    if(!status.bit.Init_done);
    else
    {
        if(Meas.U_dc_0 < Meas_alarm_L.U_dc) alarm.bit.U_dc_0_L = 1;
        if(Meas.U_dc_0 > Meas_alarm_H.U_dc) alarm.bit.U_dc_0_H = 1;
        //if(Meas.U_dc_1 < Meas_alarm_L.U_dc) alarm.bit.U_dc_1_L = 1;
        //if(Meas.U_dc_1 > Meas_alarm_H.U_dc) alarm.bit.U_dc_1_H = 1;
        //if(Meas.U_dc_2 < Meas_alarm_L.U_dc) alarm.bit.U_dc_2_L = 1;
        //if(Meas.U_dc_2 > Meas_alarm_H.U_dc) alarm.bit.U_dc_2_H = 1;
        //if(Meas.U_dc_3 < Meas_alarm_L.U_dc) alarm.bit.U_dc_3_L = 1;
        //if(Meas.U_dc_3 > Meas_alarm_H.U_dc) alarm.bit.U_dc_3_H = 1;

        if(Meas.I_conv_0 < Meas_alarm_L.I_conv) alarm.bit.I_conv_0_L = 1;
        if(Meas.I_conv_0 > Meas_alarm_H.I_conv) alarm.bit.I_conv_0_H = 1;
        //if(Meas.I_conv_1 < Meas_alarm_L.I_conv) alarm.bit.I_conv_1_L = 1;
        //if(Meas.I_conv_1 > Meas_alarm_H.I_conv) alarm.bit.I_conv_1_H = 1;
        //if(Meas.I_conv_2 < Meas_alarm_L.I_conv) alarm.bit.I_conv_2_L = 1;
        //if(Meas.I_conv_2 > Meas_alarm_H.I_conv) alarm.bit.I_conv_2_H = 1;
        //if(Meas.I_conv_3 < Meas_alarm_L.I_conv) alarm.bit.I_conv_3_L = 1;
        //if(Meas.I_conv_3 > Meas_alarm_H.I_conv) alarm.bit.I_conv_3_H = 1;

        if(Meas.I_inv_0 < Meas_alarm_L.I_inv) alarm.bit.I_inv_0_L = 1;
        if(Meas.I_inv_0 > Meas_alarm_H.I_inv) alarm.bit.I_inv_0_H = 1;
        //if(Meas.I_inv_1 < Meas_alarm_L.I_inv) alarm.bit.I_inv_1_L = 1;
        //if(Meas.I_inv_1 > Meas_alarm_H.I_inv) alarm.bit.I_inv_1_H = 1;
        //if(Meas.I_inv_2 < Meas_alarm_L.I_inv) alarm.bit.I_inv_2_L = 1;
        //if(Meas.I_inv_2 > Meas_alarm_H.I_inv) alarm.bit.I_inv_2_H = 1;
        //if(Meas.I_inv_3 < Meas_alarm_L.I_inv) alarm.bit.I_inv_3_L = 1;
        //if(Meas.I_inv_3 > Meas_alarm_H.I_inv) alarm.bit.I_inv_3_H = 1;

        if(Meas.I_grid < Meas_alarm_L.I_grid) alarm.bit.I_grid_L = 1;
        if(Meas.I_grid > Meas_alarm_H.I_grid) alarm.bit.I_grid_H = 1;

        if(Meas.temperature_0A > Meas_alarm_H.temperature) alarm.bit.temperature_0A_H = 1;
        if(Meas.temperature_0B > Meas_alarm_H.temperature) alarm.bit.temperature_0B_H = 1;
        if(Meas.temperature_1A > Meas_alarm_H.temperature) alarm.bit.temperature_1A_H = 1;
        if(Meas.temperature_1B > Meas_alarm_H.temperature) alarm.bit.temperature_1B_H = 1;
        if(Meas.temperature_2A > Meas_alarm_H.temperature) alarm.bit.temperature_2A_H = 1;
        if(Meas.temperature_2B > Meas_alarm_H.temperature) alarm.bit.temperature_2B_H = 1;
        if(Meas.temperature_3A > Meas_alarm_H.temperature) alarm.bit.temperature_3A_H = 1;
        if(Meas.temperature_3B > Meas_alarm_H.temperature) alarm.bit.temperature_3B_H = 1;

        if(!CpuSysRegs.RESC.bit.TRSTn_pin_status)
        {
            EALLOW;
            EPwm9Regs.TZFRC.bit.OST = 1;
            EDIS;
        }
        // the hierarchy alarm bits

        if((alarm.all[0] | alarm.all[1]) && !(alarm_snapshot.all[0] | alarm_snapshot.all[1]))
        {
            EALLOW;
            EPwm9Regs.TZFRC.bit.OST = 1;
            EDIS;

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

            Scope.acquire_before_trigger = 1000;

            Scope.data_in[0] = &INV.duty[0];
            Scope.data_in[1] = &INV.duty[2];
            Scope.data_in[2] = &Meas.I_inv_0;
            Scope.data_in[3] = &Meas.I_inv_1;
            Scope.data_in[4] = &Meas.I_grid;
            Scope.data_in[5] = &INV.I_cross_0;
            Scope.data_in[6] = &INV.I_cross_1;
            Scope.data_in[7] = &Meas.U_grid;
            Scope.data_in[8] = &Meas.U_dc_0;
            Scope.data_in[9] = &Meas.U_dc_1;
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
            static volatile float ref_last = 0.0f;
           if(ref_last != INV.I_arm_dc_ref) Scope_trigger_unc();
           ref_last = INV.I_arm_dc_ref;
            //if(INV.state == INV_active) Scope_trigger_unc();
            //if(alarm.all[0] | alarm.all[1]) Scope_trigger_unc();
            Scope_task();
            decimation = 0;
        }
    }

    Timer_PWM.CPU_END_HWI = TIMESTAMP_PWM;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
