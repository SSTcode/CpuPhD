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
#include <Software/FLASH.h>

struct calibration_struct calibration;

class FLASH_class calibration_FLASH =
{
 .address = {(Uint16 *)&calibration, 0},
 .sector = SectorN,
 .size16_each = {sizeof(calibration), 0},
};

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

    if(control.fields.control_bits.test_save_to_flash)
    {
        control.fields.control_bits.test_save_to_flash = 0;

        calibration.Meas_gain = Meas_gain;
        calibration.Meas_offset = Meas_offset;
        calibration.available = 1;
        calibration_FLASH.save();
    }

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
        memset(&calibration, 0, sizeof(calibration));

        calibration_FLASH.retrieve();

        if(!calibration.available)
        {
            alarm.bit.Not_enough_data = 1;

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
            Meas_gain.I_inv_0 = i_meas_gain;

            Meas_gain.I_grid = i_meas_gain;
            Meas_gain.U_grid = u_meas_gain;

            Meas_gain.temperature = 0.01f;

            INV.duty[0] =
            INV.duty[1] =
            INV.duty[3] =
            INV.duty[4] =
            INV.duty[5] =
            INV.duty[6] =
            INV.duty[7] = 0.0f;
        }
        else
        {
            Meas_gain = calibration.Meas_gain;
            Meas_offset = calibration.Meas_offset;
        }

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
        INV.PR_I_arm0.x0 =
        INV.PR_I_arm0.x1 =
        INV.PR_I_arm1.x0 =
        INV.PR_I_arm1.x1 =
        INV.PR_I_arm2.x0 =
        INV.PR_I_arm2.x1 =
        INV.PR_I_arm3.x0 =
        INV.PR_I_arm3.x1 = 0.0f;
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

    case Machine_calibrate_offsets:
    {
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;
        }

        if(control.fields.control_bits.try_calibrate)
        {
            control.fields.control_bits.try_calibrate = 0;
            memcpy(&SD_card.calibration.Meas_offset, &Meas_offset, sizeof(Meas_offset));
            memcpy(&SD_card.calibration.Meas_gain, &Meas_gain, sizeof(Meas_gain));

//            CIC2_calibration_input.ptr = &Meas.I_conv_0;
//            DELAY_US(100000);
//            Meas_offset.I_conv_0 += CIC2_calibration.out / Meas_gain.I_conv_0;
//
//            CIC2_calibration_input.ptr = &Meas.I_conv_1;
//            DELAY_US(100000);
//            Meas_offset.I_conv_1 += CIC2_calibration.out / Meas_gain.I_conv_1;
//
//            CIC2_calibration_input.ptr = &Meas.I_conv_2;
//            DELAY_US(100000);
//            Meas_offset.I_conv_2 += CIC2_calibration.out / Meas_gain.I_conv_2;
//
//            CIC2_calibration_input.ptr = &Meas.I_conv_3;
//            DELAY_US(100000);
//            Meas_offset.I_conv_3 += CIC2_calibration.out / Meas_gain.I_conv_3;
//
//
//            CIC2_calibration_input.ptr = &Meas.I_inv_0;
//            DELAY_US(100000);
//            Meas_offset.I_inv_0 += CIC2_calibration.out / Meas_gain.I_inv_0;
//
//            CIC2_calibration_input.ptr = &Meas.I_inv_1;
//            DELAY_US(100000);
//            Meas_offset.I_inv_1 += CIC2_calibration.out / Meas_gain.I_inv_1;
//
//            CIC2_calibration_input.ptr = &Meas.I_inv_2;
//            DELAY_US(100000);
//            Meas_offset.I_inv_2 += CIC2_calibration.out / Meas_gain.I_inv_2;
//
//            CIC2_calibration_input.ptr = &Meas.I_inv_3;
//            DELAY_US(100000);
//            Meas_offset.I_inv_3 += CIC2_calibration.out / Meas_gain.I_inv_3;
//
//
//            CIC2_calibration_input.ptr = &Meas.I_grid;
//            DELAY_US(100000);
//            Meas_offset.I_grid += CIC2_calibration.out / Meas_gain.I_grid;
//
//            CIC2_calibration_input.ptr = &Meas.U_grid;
//            DELAY_US(100000);
//            Meas_offset.U_grid += CIC2_calibration.out / Meas_gain.U_grid;


            CIC2_calibration_input.ptr = &Meas.U_dc_0;
            DELAY_US(100000);
            Meas_offset.U_dc_0 += CIC2_calibration.out / Meas_gain.U_dc_0;

            CIC2_calibration_input.ptr = &Meas.U_dc_1;
            DELAY_US(100000);
            Meas_offset.U_dc_1 += CIC2_calibration.out / Meas_gain.U_dc_1;

            CIC2_calibration_input.ptr = &Meas.U_dc_2;
            DELAY_US(100000);
            Meas_offset.U_dc_2 += CIC2_calibration.out / Meas_gain.U_dc_2;

            CIC2_calibration_input.ptr = &Meas.U_dc_3;
            DELAY_US(100000);
            Meas_offset.U_dc_3 += CIC2_calibration.out / Meas_gain.U_dc_3;


            static struct Measurements_struct Meas_offset_real;
            Meas_offset_real.U_dc_0   = fabsf(Meas_offset.U_dc_0 * Meas_gain.U_dc_0);
            Meas_offset_real.U_dc_1   = fabsf(Meas_offset.U_dc_1 * Meas_gain.U_dc_1);
            Meas_offset_real.U_dc_2   = fabsf(Meas_offset.U_dc_2 * Meas_gain.U_dc_2);
            Meas_offset_real.U_dc_3   = fabsf(Meas_offset.U_dc_3 * Meas_gain.U_dc_3);
            Meas_offset_real.I_conv_0 = fabsf(Meas_offset.I_conv_0 * Meas_gain.I_conv_0);
            Meas_offset_real.I_conv_1 = fabsf(Meas_offset.I_conv_1 * Meas_gain.I_conv_1);
            Meas_offset_real.I_conv_2 = fabsf(Meas_offset.I_conv_2 * Meas_gain.I_conv_2);
            Meas_offset_real.I_conv_3 = fabsf(Meas_offset.I_conv_3 * Meas_gain.I_conv_3);
            Meas_offset_real.I_inv_0  = fabsf(Meas_offset.I_inv_0 * Meas_gain.I_inv_0);
            Meas_offset_real.I_inv_1  = fabsf(Meas_offset.I_inv_1 * Meas_gain.I_inv_1);
            Meas_offset_real.I_inv_2  = fabsf(Meas_offset.I_inv_2 * Meas_gain.I_inv_2);
            Meas_offset_real.I_inv_3  = fabsf(Meas_offset.I_inv_3 * Meas_gain.I_inv_3);
            Meas_offset_real.U_grid   = fabsf(Meas_offset.U_grid * Meas_gain.U_grid);
            Meas_offset_real.I_grid   = fabsf(Meas_offset.I_grid * Meas_gain.I_grid);

            float tolerance_U = 1.0f;
            float tolerance_I = 0.05f;
            if (Meas_offset_real.U_dc_0 > tolerance_U ||
                Meas_offset_real.U_dc_1 > tolerance_U ||
                Meas_offset_real.U_dc_2 > tolerance_U ||
                Meas_offset_real.U_dc_3 > tolerance_U ||
                Meas_offset_real.I_conv_0 > tolerance_I ||
                Meas_offset_real.I_conv_1 > tolerance_I ||
                Meas_offset_real.I_conv_2 > tolerance_I ||
                Meas_offset_real.I_conv_3 > tolerance_I ||
                Meas_offset_real.I_inv_0 > tolerance_I ||
                Meas_offset_real.I_inv_1 > tolerance_I ||
                Meas_offset_real.I_inv_2 > tolerance_I ||
                Meas_offset_real.I_inv_3 > tolerance_I ||
                Meas_offset_real.U_grid > tolerance_U ||
                Meas_offset_real.I_grid > tolerance_I)
            {
                status.bit.calibration_error = 1;
            }
            else
            {
                status.bit.calibration_error = 0;
                Machine.state = Machine_calibrate_gain;
            }
        }
        break;
    }

    case Machine_calibrate_gain:
    {
        static Uint16 calib_rdy;
        static Uint16 calib_rdy_last;
        if(Machine.state_last != Machine.state)
        {
            Machine.state_last = Machine.state;
            calib_rdy = 0;
            calib_rdy_last = 0;
        }

        if(control.fields.control_bits.try_calibrate)
        {
            control.fields.control_bits.try_calibrate = 0;

            float I_cal = 2.0f;
            float U_cal = 28.0f;

//            if(fabsf(Meas.I_conv_0) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_conv_0;
//                DELAY_US(100000);
//                Meas_gain.I_conv_0 = fabsf(Meas_gain.I_conv_0 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<0;
//            }
//            if(fabsf(Meas.I_conv_1) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_conv_1;
//                DELAY_US(100000);
//                Meas_gain.I_conv_1 = fabsf(Meas_gain.I_conv_1 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<1;
//            }
//            if(fabsf(Meas.I_conv_2) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_conv_2;
//                DELAY_US(100000);
//                Meas_gain.I_conv_2 = fabsf(Meas_gain.I_conv_2 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<2;
//            }
//            if(fabsf(Meas.I_conv_3) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_conv_3;
//                DELAY_US(100000);
//                Meas_gain.I_conv_3 = fabsf(Meas_gain.I_conv_3 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<3;
//            }
//
//            if(fabsf(Meas.I_inv_0) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_inv_0;
//                DELAY_US(100000);
//                Meas_gain.I_inv_0 = fabsf(Meas_gain.I_inv_0 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<4;
//            }
//            if(fabsf(Meas.I_inv_1) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_inv_1;
//                DELAY_US(100000);
//                Meas_gain.I_inv_1 = fabsf(Meas_gain.I_inv_1 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<5;
//            }
//            if(fabsf(Meas.I_inv_2) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_inv_2;
//                DELAY_US(100000);
//                Meas_gain.I_inv_2 = fabsf(Meas_gain.I_inv_2 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<6;
//            }
//            if(fabsf(Meas.I_inv_3) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_inv_3;
//                DELAY_US(100000);
//                Meas_gain.I_inv_3 = fabsf(Meas_gain.I_inv_3 * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<7;
//            }
//
//            if(fabsf(Meas.U_grid) > U_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.U_grid;
//                DELAY_US(100000);
//                Meas_gain.U_grid = fabsf(Meas_gain.U_grid * 30.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<12;
//            }
//            if(fabsf(Meas.I_grid) > I_cal)
//            {
//                CIC2_calibration_input.ptr = &Meas.I_grid;
//                DELAY_US(100000);
//                Meas_gain.I_grid = fabsf(Meas_gain.I_grid * 5.0f / CIC2_calibration.out);
//                calib_rdy |= 1<<13;
//            }

            if(fabsf(Meas.U_dc_0) > U_cal)
            {
                CIC2_calibration_input.ptr = &Meas.U_dc_0;
                DELAY_US(100000);
                Meas_gain.U_dc_0 = -fabsf(Meas_gain.U_dc_0 * 30.0f / CIC2_calibration.out);
                calib_rdy |= 1<<8;
            }
            if(fabsf(Meas.U_dc_1) > U_cal)
            {
                CIC2_calibration_input.ptr = &Meas.U_dc_1;
                DELAY_US(100000);
                Meas_gain.U_dc_1 = -fabsf(Meas_gain.U_dc_1 * 30.0f / CIC2_calibration.out);
                calib_rdy |= 1<<9;
            }
            if(fabsf(Meas.U_dc_2) > U_cal)
            {
                CIC2_calibration_input.ptr = &Meas.U_dc_2;
                DELAY_US(100000);
                Meas_gain.U_dc_2 = -fabsf(Meas_gain.U_dc_2 * 30.0f / CIC2_calibration.out);
                calib_rdy |= 1<<10;
            }
            if(fabsf(Meas.U_dc_3) > U_cal)
            {
                CIC2_calibration_input.ptr = &Meas.U_dc_3;
                DELAY_US(100000);
                Meas_gain.U_dc_3 = -fabsf(Meas_gain.U_dc_3 * 30.0f / CIC2_calibration.out);
                calib_rdy |= 1<<11;
            }

            if(calib_rdy == calib_rdy_last)
            {
                status.bit.calibration_error = 1;
            }
            calib_rdy_last = calib_rdy;

            if(calib_rdy == 0xF00)
            {
                float mean_gain_meas;

                static struct Measurements_struct Meas_gain_ratio;
                mean_gain_meas = (Meas_gain.U_dc_0 + Meas_gain.U_dc_1 + Meas_gain.U_dc_2 + Meas_gain.U_dc_3) / 4.0f;
                Meas_gain_ratio.U_dc_0   = fabsf(Meas_gain.U_dc_0 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.U_dc_1   = fabsf(Meas_gain.U_dc_1 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.U_dc_2   = fabsf(Meas_gain.U_dc_2 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.U_dc_3   = fabsf(Meas_gain.U_dc_3 / mean_gain_meas) - 1.0f;
                mean_gain_meas = (Meas_gain.I_conv_0 + Meas_gain.I_conv_1 + Meas_gain.I_conv_2 + Meas_gain.I_conv_3) / 4.0f;
                Meas_gain_ratio.I_conv_0 = fabsf(Meas_gain.I_conv_0 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_conv_1 = fabsf(Meas_gain.I_conv_1 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_conv_2 = fabsf(Meas_gain.I_conv_2 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_conv_3 = fabsf(Meas_gain.I_conv_3 / mean_gain_meas) - 1.0f;
                mean_gain_meas = (Meas_gain.I_inv_0 + Meas_gain.I_inv_1 + Meas_gain.I_inv_2 + Meas_gain.I_inv_3) / 4.0f;
                Meas_gain_ratio.I_inv_0  = fabsf(Meas_gain.I_inv_0 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_inv_1  = fabsf(Meas_gain.I_inv_1 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_inv_2  = fabsf(Meas_gain.I_inv_2 / mean_gain_meas) - 1.0f;
                Meas_gain_ratio.I_inv_3  = fabsf(Meas_gain.I_inv_3 / mean_gain_meas) - 1.0f;

                Meas_gain_ratio.U_grid   = fabsf(Meas_offset.U_grid / SD_card.calibration.Meas_gain.U_grid);
                Meas_gain_ratio.I_grid   = fabsf(Meas_offset.I_grid / SD_card.calibration.Meas_gain.I_grid);

                float tolerance = 0.03;
                if (Meas_gain_ratio.U_dc_0 > tolerance ||
                    Meas_gain_ratio.U_dc_1 > tolerance ||
                    Meas_gain_ratio.U_dc_2 > tolerance ||
                    Meas_gain_ratio.U_dc_3 > tolerance ||
                    Meas_gain_ratio.I_conv_0 > tolerance ||
                    Meas_gain_ratio.I_conv_1 > tolerance ||
                    Meas_gain_ratio.I_conv_2 > tolerance ||
                    Meas_gain_ratio.I_conv_3 > tolerance ||
                    Meas_gain_ratio.I_inv_0 > tolerance ||
                    Meas_gain_ratio.I_inv_1 > tolerance ||
                    Meas_gain_ratio.I_inv_2 > tolerance ||
                    Meas_gain_ratio.I_inv_3 > tolerance ||
                    Meas_gain_ratio.U_grid > tolerance ||
                    Meas_gain_ratio.I_grid > tolerance)
                {
                    status.bit.calibration_error = 1;
                }
                else
                {
                    status.bit.calibration_error = 0;
                    Machine.state = Machine_idle;
                }
            }
        }
        break;
    }

    default:
    {
        Machine.state = Machine_init;
        break;
    }

    }
}

