/*
 * State.cpp
 *
 *  Created on: 2 maj 2020
 *      Author: MrTea
 */
#include "stdafx.h"
#include <State.h>
#include <Scope.h>
#include <SD_card.h>
#include <Init.h>
#include <Blink.h>
#include <string.h>

struct Machine_struct Machine;

void Blink()
{
    static class Blink_class Blink_LED1(false);
    static class Blink_class Blink_LED2(false);
    static class Blink_class Blink_LED3(false);
    static class Blink_class Blink_LED4(false);
    static class Blink_class Blink_LED5(false);

    Blink_LED1.update_pattern(2.0f, 0.67f);
    Blink_LED2.update_pattern(1.0f, 0.5f);
    static const float pattern[] = {0.1f, -0.2f, 0.3f, -1.3f, -1.3f};
    static const float period = 1.3f;
    Blink_LED3.update_pattern(period, (float *)pattern);
    Blink_LED4.update_pattern(true);
    Blink_LED5.update_pattern(true);

    GPIO_WRITE(LED1_ON_CPU, Blink_LED1.task());
    GPIO_WRITE(LED2_ON_CPU, Blink_LED2.task());
    GPIO_WRITE(LED3_ON_CPU, Blink_LED3.task());
}

void SMachine_Background()
{
    Blink();

    Init.IPCBootCPU2_flash();

//    if(control.fields.control_bits.SD_save_settings)
//    {
//        control.fields.control_bits.SD_save_settings = 0;
//
//        SD_card.settings.control = control;
//        SD_card.settings.available = 1;
//
//        if(!SD_card.save_settings()) status.bit.SD_no_settings = 0;
//        else status.bit.SD_no_settings = 1;
//    }

    if(control.fields.control_bits.CPU_reset)
    {
        control.fields.control_bits.CPU_reset = 0;

        EALLOW;
        NmiIntruptRegs.NMICFG.bit.NMIE = 1;
        NmiIntruptRegs.NMIWDPRD = 0;
        NmiIntruptRegs.NMIFLGFRC.bit.OVF = 1;
        EDIS;
    }
}

void SMachine_Main()
{
    switch(Machine.state)
    {

    case Machine_init:
    {
        memset(&Machine, 0, sizeof(Machine)); //copies the character 0 to the first n characters sizeof(Machine) of the string pointed to, by the argument &Machine.
        memset(&Meas, 0, sizeof(Meas));
        memset(&Meas_offset, 0, sizeof(Meas_offset));
        memset(&Meas_gain, 0, sizeof(Meas_gain));
        memset(&Meas_alarm_H, 0, sizeof(Meas_alarm_H));
        memset(&Meas_alarm_L, 0, sizeof(Meas_alarm_L));
        memset(&control, 0, sizeof(control));
        memset(&status, 0, sizeof(status));
        memset(&alarm, 0, sizeof(alarm));
        memset(&alarm_snapshot, 0, sizeof(alarm_snapshot));

//        SD_card.read_settings();
//        SD_card.read_calibration_data();

      // if(!SD_card.settings.available || !SD_card.calibration.available)
      //     status.bit.SD_card_not_enough_data = 1;
      // else
      //     status.bit.SD_card_not_enough_data = 0;
      //
      // if(!SD_card.calibration.available) status.bit.SD_no_calibration = 1;
      // if(!SD_card.settings.available) status.bit.SD_no_settings = 1;
      //
       // if(status.bit.SD_card_not_enough_data)
       // {
       //     alarm.bit.Not_enough_data = 1;

            float OSR = 400;
            float truncation = 8.0f;

            //1/(OSR*OSR)
            //*0.064V
            ///0.002Ohm
            //*8.0 truncation to 16bits
            float i_meas_gain = 0.064/(OSR*OSR)/0.002*truncation;

            //2/(200*200) = 0.0000125
            //*0.064V = 0.0000032

            ///0.002Ohm = 0.0016 //shunt resistor
           // float i_meas_gain = (2.0/(200.0*200.0)*0.064)/0.002;//0.0016;


            //1/(OSR*OSR)
            //*0.064V
            //Divider = 390/(3M + 390)
            //Gain error R3/Rin = 1 + (390*2)/4.9k
            //*8.0 truncation to 16bits

            float r_down = (390.0 * 4.9e3)/(390.0 + 4.9e3);
            float u_meas_gain = 10.0f/10.9f*0.064/(OSR*OSR)*(3e6 + r_down)/(r_down)*(1.0 + (390.0/2.5e3))*truncation;
            //390/(1M*3 + 390) //Divider
            //(2.5/(2.5+0.39)) //R3 error
           // float u_meas_gain = (2.0/(200.0*200.0)*0.064)/(0.39/(1000.0*3.0 + 0.39))/(2.5/(2.5+0.39));

            Meas_gain.U_dc_3 =
            Meas_gain.U_dc_2 =
            Meas_gain.U_dc_1 =
            Meas_gain.U_dc_0 = -u_meas_gain;

            Meas_gain.I_conv_3 =
            Meas_gain.I_conv_2 =
            Meas_gain.I_conv_1 =
            Meas_gain.I_conv_0 = i_meas_gain;

            Meas_gain.I_inv_3 =
            Meas_gain.I_inv_2 =
            Meas_gain.I_inv_1 =
            Meas_gain.I_inv_0 = i_meas_gain*1,84;

            Meas_gain.I_grid = i_meas_gain;
            Meas_gain.U_grid = u_meas_gain;

            Meas_gain.temperature = 0.01f;
       //}
       // else
       // {
       //     Meas_gain = SD_card.calibration.Meas_gain;
       //     Meas_offset = SD_card.calibration.Meas_offset;
       // }

        Init.Variables();

        Init.clear_alarms();

        Scope_start();

        EALLOW;
        Cla1Regs.MIER.bit.INT1 =
        Cla1Regs.MICLR.bit.INT1 = 1;
        EDIS;

        PieCtrlRegs.PIEIFR1.all =
        PieCtrlRegs.PIEIFR2.all =
        PieCtrlRegs.PIEIFR3.all =
        PieCtrlRegs.PIEIFR4.all =
        PieCtrlRegs.PIEIFR5.all =
        PieCtrlRegs.PIEIFR6.all =
        PieCtrlRegs.PIEIFR7.all =
        PieCtrlRegs.PIEIFR8.all =
        PieCtrlRegs.PIEIFR9.all =
        PieCtrlRegs.PIEIFR10.all =
        PieCtrlRegs.PIEIFR11.all =
        PieCtrlRegs.PIEIFR12.all = 0;
        PieCtrlRegs.PIEACK.all = 0xFFFF;
        IFR &= 0;
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");
        __asm(" NOP");

        EINT;

        status.bit.Init_done = 1;
        Machine.state = Machine_idle;
        break;
    }

    case Machine_idle:
    {
        INV.enable = 0.0f;
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;
        }
        INV.PR_I_arm.y0 =
        INV.PR_I_arm.x0 =
        INV.PR_I_arm.x1 =
        INV.PR_I_arm2.y0 =
        INV.PR_I_arm2.x0 =
        INV.PR_I_arm2.x1 = 0.0f;
        //Machine.state = Machine_start;
        break;
    }

    case Machine_start:
    {
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;

            Init.clear_alarms();

            DELAY_US(100);

            Scope_start();
            INV.enable = 1.0f;
        }
        if(INV.RDY)
        {
            Machine.state = Machine_operational;
        }
        if(!1) Machine.state = Machine_idle;
        if(alarm.all[0] | alarm.all[1]) Machine.state = Machine_cleanup;//alarm is it present
        break;
    }


    case Machine_operational:
    {
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;
        }

        if(!1) Machine.state = Machine_idle;
        if(alarm.all[0] | alarm.all[1]) Machine.state = Machine_cleanup;
        break;
    }

    case Machine_cleanup:
    {
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;
            Scope_trigger_unc();
        }

        Machine.state = Machine_idle;
        break;
    }

    default:
    {
        Machine.state = Machine_init;
        break;
    }

    }
}

