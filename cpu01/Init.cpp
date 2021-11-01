/*
 * Hardware.c
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <stdafx.h>
#include "HWIs.h"
#include <math.h>
#include "F2837xD_sdfm_drivers.h"
#include "F2837xD_Ipc_drivers.h"
#include <Init.h>
#include <string.h>
#include <State.h>

class Init_class Init;

void Init_class::IPCBootCPU2_flash()
{
    if(( (IpcRegs.IPCBOOTSTS & 0x0000000F) == C2_BOOTROM_BOOTSTS_SYSTEM_READY)
        && ((IpcRegs.IPCFLG.all & (IPC_FLAG0 | IPC_FLAG31)) == 0))
    {
        //
        //CPU01 to CPU02 IPC Boot Mode Register
        //
        IpcRegs.IPCBOOTMODE = C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH;
        //
        // CPU01 To CPU02 IPC Command Register
        //
        IpcRegs.IPCSENDCOM  = BROM_IPC_EXECUTE_BOOTMODE_CMD;
        //
        // CPU01 to CPU02 IPC flag register
        //
        IpcRegs.IPCSET.all = (IPC_FLAG0 | IPC_FLAG31);
    }
    else
    {
        IpcRegs.IPCCLR.all = (IPC_FLAG0 | IPC_FLAG31);
    }
}

void Init_class::CLA()
{
    EALLOW;

    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;
    //
    // Initialize and wait for CLA1ToCPUMsgRAM
    //
    MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};
    //
    // Initialize and wait for CPUToCLA1MsgRAM
    //
    MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};
    //
    // Select LS4RAM and LS5RAM to be the programming space for the CLA
    // First configure the CLA to be the master for LS4 and LS5 and then
    // set the space to be a program block
    //
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 1;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 1;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 1;
    //
    // Next configure LS0RAM and LS1RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    //
    MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 0;
    EDIS;


    EALLOW;
    Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);

    Cla1Regs.MVECT2 =
    Cla1Regs.MVECT3 =
    Cla1Regs.MVECT4 =
    Cla1Regs.MVECT5 =
    Cla1Regs.MVECT6 =
    Cla1Regs.MVECT7 =
    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task1);

//    Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
//    Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
//    Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
//    Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
//    Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
//    Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
//    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 0;

    Cla1Regs.MCTL.bit.IACKE = 1;

    Cla1Regs.MIER.bit.INT1 = 0;
    Cla1Regs.MIER.bit.INT2 = 0;
    Cla1Regs.MIER.bit.INT3 = 0;
    Cla1Regs.MIER.bit.INT4 = 0;
    Cla1Regs.MIER.bit.INT5 = 0;
    Cla1Regs.MIER.bit.INT6 = 0;
    Cla1Regs.MIER.bit.INT7 = 0;
    Cla1Regs.MIER.bit.INT8 = 0;
    EDIS;
}

void Init_class::CPUS()
{
    EALLOW;

    DevCfgRegs.CPUSEL0.bit.EPWM11 = 1;     // CPU Select register for common peripherals

    MemCfgRegs.GSxMSEL.bit.MSEL_GS0  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS1  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS2  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS3  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS4  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS5  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS6  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS7  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS8  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS9  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS10 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS11 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS12 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS13 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS15 = 1;

   EDIS;
}

void Init_class::EMIF()
{
    GPIO_Setup(EM1D0 );
    GPIO_Setup(EM1D1 );
    GPIO_Setup(EM1D2 );
    GPIO_Setup(EM1D3 );
    GPIO_Setup(EM1D4 );
    GPIO_Setup(EM1D5 );
    GPIO_Setup(EM1D6 );
    GPIO_Setup(EM1D7 );
    GPIO_Setup(EM1D8 );
    GPIO_Setup(EM1D9 );
    GPIO_Setup(EM1D10);
    GPIO_Setup(EM1D11);
    GPIO_Setup(EM1D12);
    GPIO_Setup(EM1D13);
    GPIO_Setup(EM1D14);
    GPIO_Setup(EM1D15);

    GPIO_Setup(EM1CS2); // CS2 low when operation active
    GPIO_Setup(EM1WE ); // WE low when write active
    GPIO_Setup(EM1OE ); // OE low when read active

    //GPIO_Setup(EM1CLK ); // CLK

    GPIO_Setup(EM1BA1);
    GPIO_Setup(EM1A0 );
    GPIO_Setup(EM1A1 );
    GPIO_Setup(EM1A2 );
    GPIO_Setup(EM1A3 );
    GPIO_Setup(EM1A4 );
    GPIO_Setup(EM1A5 );
    GPIO_Setup(EM1A6 );
    GPIO_Setup(EM1A7 );
    GPIO_Setup(EM1A8 );
    GPIO_Setup(EM1A9 );
    GPIO_Setup(EM1A10);
    GPIO_Setup(EM1A11);

    Uint16  ErrCount = 0;
    //
    // Configure to run EMIF1 on full Rate. (EMIF1CLK = CPU1SYSCLK). SysCtl_setEMIF1ClockDivider
    //
EALLOW;
    CpuSysRegs.PCLKCR1.bit.EMIF1            = 0x1;//EMIF1 Clock Enable bit: 1: Module clock is turned-on
    ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x0;//EMIF1 module run with a /1 or /2 clock. For single core device. 0: /1 of CPU1.SYSCLK is selected. For Dual core device 0: /1 of PLLSYSCLK is selected
    //
    // Grab EMIF1 For CPU1. EMIF_selectMaster
    //
    Emif1ConfigRegs.EMIF1MSEL.all = 0x93A5CE71; //Writing the value 0x93A5CE7 will allow the writing of the  EMIF1M select bits
    if(Emif1ConfigRegs.EMIF1MSEL.all != 0x1)
    {
        ErrCount++;
    }
    //Disable Access Protection (CPU_FETCH/CPU_WR/DMA_WR)
    Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x0;
    if(Emif1ConfigRegs.EMIF1ACCPROT0.all != 0x0)
    {
        ErrCount++;
    }
    // Commit the configuration related to protection. Till this bit remains set
    // content of EMIF1ACCPROT0 register can't be changed.
    Emif1ConfigRegs.EMIF1COMMIT.all = 0x1;
    if(Emif1ConfigRegs.EMIF1COMMIT.all != 0x1)
    {
       ErrCount++;
    }
    // Lock the configuration so that EMIF1COMMIT register can't be
    // changed any more.
    Emif1ConfigRegs.EMIF1LOCK.all = 0x1;
    if(Emif1ConfigRegs.EMIF1LOCK.all != 1)
    {
        ErrCount++;
    }
    // Async 1 (EMxCS2n) Config Register the access timing for CS2 space
    Emif1Regs.ASYNC_CS2_CR.bit.SS       = 0;//Select Strobe mode. Set to 1 if chip selects need to have write or read strobe timing
    Emif1Regs.ASYNC_CS2_CR.bit.EW       = 0;//Extend Wait mode. Set to 1 if extended asynchronous cycles are required based on EMxWAIT.
    Emif1Regs.ASYNC_CS2_CR.bit.W_SETUP  = 0;//Write Strobe Setup cycles
    Emif1Regs.ASYNC_CS2_CR.bit.W_STROBE = 0;//Write Strobe Duration cycles
    Emif1Regs.ASYNC_CS2_CR.bit.W_HOLD   = 0;//Write Strobe Hold cycles
    Emif1Regs.ASYNC_CS2_CR.bit.R_SETUP  = 0;//Read Strobe Setup cycles.
    Emif1Regs.ASYNC_CS2_CR.bit.R_STROBE = 4;//Read Strobe Duration cycles. Number of EMxCLK cycles for which EMxOEn is held active,
    Emif1Regs.ASYNC_CS2_CR.bit.R_HOLD   = 0;//Read Strobe Hold cycles
    Emif1Regs.ASYNC_CS2_CR.bit.TA       = 0;// Turn Around time of 2 Emif Clock
    Emif1Regs.ASYNC_CS2_CR.bit.ASIZE    = 1;//Asynchronous Memory Size. 01: 16 Bit data bus.
EDIS;
}

void Init_class::clear_alarms()
{
    Init.EPwm_TZclear(&EPwm9Regs);

    if(alarm.bit.Not_enough_data)
    {
        alarm.all[0] =
        alarm.all[1] =
        alarm_snapshot.all[0] =
        alarm_snapshot.all[1] = 0;
        alarm.bit.Not_enough_data = 1;
    }
    else
    {
        alarm.all[0] =
        alarm.all[1] =
        alarm_snapshot.all[0] =
        alarm_snapshot.all[1] = 0;
    }
}

void Init_class::TZ_EN(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = 1999;                   // PWM frequency = 1/(TBPRD+1)
    EPwmReg->TBCTR = 0;                     //clear counter
    EPwmReg->TBPHS.all = 0;

    EPwmReg->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwmReg->TBCTL.bit.PHSEN = TB_ENABLE;
    EPwmReg->TBCTL.bit.PHSDIR = TB_UP;

//Configure modes, clock dividers and action qualifier
    EPwmReg->TBCTL.bit.CTRMODE = TB_COUNT_UP;         // Select up-down count mode
    EPwmReg->TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwmReg->TBCTL.bit.CLKDIV = TB_DIV1;                  // TBCLK = SYSCLKOUT
    EPwmReg->TBCTL.bit.FREE_SOFT = 2;
    EPwmReg->TBCTL.bit.PRDLD = TB_SHADOW;                 // set Shadow load

    EPwmReg->AQCTLA.bit.ZRO = AQ_SET;
    EPwmReg->AQCTLA.bit.PRD = AQ_SET;

    //Configure trip-zone
    EPwmReg->TZCTL.bit.TZA = TZ_FORCE_LO;

    //    EPwmReg->TZSEL.bit.OSHT1 = 1;
//    EPwmReg->TZSEL.bit.OSHT3 = 1;
    EPwmReg->TZSEL.bit.OSHT5 = 1;//ESTOP0 protection - against breakpoints
    EPwmReg->TZSEL.bit.OSHT6 = 1;
    EDIS;
}

void Init_class::EPwm_TZclear(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;
    EPwmReg->TZOSTCLR.all = 0xFF;
    EPwmReg->TZCLR.bit.OST = 1;
    EDIS;
}

void Init_class::EPwm(volatile struct EPWM_REGS *EPwmReg)
{
    EPwmReg->TBPRD = 1000;                   // PWM frequency = 1/(2*TBPRD)
    EPwmReg->TBPHS.bit.TBPHS = 1;
    EPwmReg->TBCTL.bit.PHSDIR = TB_DOWN;

    EPwmReg->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwmReg->TBCTL.bit.PHSEN = TB_ENABLE;

    EPwmReg->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwmReg->TBCTL.bit.HSPCLKDIV = TB_DIV1;

    EALLOW;
    EPwmReg->GLDCTL.bit.GLD = 1;
    EPwmReg->GLDCTL.bit.GLDPRD = 1;
    EPwmReg->GLDCTL.bit.GLDMODE = 6;
    EPwmReg->GLDCFG.bit.AQCSFRC = 1;
    EPwmReg->GLDCFG.bit.CMPA_CMPAHR = 1;
    EDIS;

    EPwmReg->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwmReg->CMPCTL.bit.SHDWBMODE = CC_SHADOW;

    EPwmReg->AQTSRCSEL.bit.T1SEL = 7;
    EPwmReg->AQCTLA2.bit.T1D = AQ_SET;
    EPwmReg->AQCTLA.bit.ZRO = AQ_SET;
    EPwmReg->AQCTLA.bit.CAD = AQ_SET;
    EPwmReg->AQCTLA.bit.CAU = AQ_CLEAR;
    EPwmReg->AQCTLA.bit.PRD = AQ_CLEAR;

    EPwmReg->DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwmReg->DBCTL.bit.SHDWDBREDMODE = 1;

    EPwmReg->DBCTL.bit.HALFCYCLE = 1;
    EPwmReg->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwmReg->DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwmReg->DBCTL.bit.IN_MODE = DBA_ALL;
    EPwmReg->DBRED.bit.DBRED = 50;
    EPwmReg->DBFED.bit.DBFED = 50;
}


void Init_class::PWM_timestamp(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = 10000-1;                   // PWM frequency = 1/(TBPRD+1)
    EPwmReg->TBPHS.all = 0;

    EPwmReg->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwmReg->TBCTL.bit.PHSEN = TB_ENABLE;
    EPwmReg->TBCTL.bit.PHSDIR = TB_UP;

//Configure modes, clock dividers and action qualifier
    EPwmReg->TBCTL.bit.CTRMODE = TB_COUNT_UP;         // Select up-down count mode
    EPwmReg->TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwmReg->TBCTL.bit.CLKDIV = TB_DIV1;                  // TBCLK = SYSCLKOUT
    EPwmReg->TBCTL.bit.FREE_SOFT = 2;
    EPwmReg->TBCTL.bit.PRDLD = TB_SHADOW;                 // set Shadow load

    EDIS;
}

void Init_class::PWMs()
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM9 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;
    EDIS;

    EALLOW;
    SyncSocRegs.SYNCSELECT.bit.EPWM4SYNCIN = 5;
    SyncSocRegs.SYNCSELECT.bit.EPWM7SYNCIN = 5;
    SyncSocRegs.SYNCSELECT.bit.EPWM10SYNCIN = 5;
    EDIS;

    EPwm(&EPwm1Regs);
    EPwm(&EPwm2Regs);
    EPwm(&EPwm3Regs);
    EPwm(&EPwm4Regs);
    EPwm(&EPwm5Regs);
    EPwm(&EPwm6Regs);
    EPwm(&EPwm7Regs);
    EPwm(&EPwm8Regs);
    TZ_EN(&EPwm9Regs);//TZ_EN_CPU signal must be high. This signal is controlled by EPwm9Regs:
    PWM_timestamp(&EPwm10Regs);

    GPIO_Setup(TZ_EN_CPU);
    GPIO_Setup(PWM_SYNC_CPU);

    GPIO_Setup(PWM1A);
    GPIO_Setup(PWM1B);
    GPIO_Setup(PWM2A);
    GPIO_Setup(PWM2B);
    GPIO_Setup(PWM3A);
    GPIO_Setup(PWM3B);
    GPIO_Setup(PWM4A);
    GPIO_Setup(PWM4B);
    GPIO_Setup(PWM5A);
    GPIO_Setup(PWM5B);
    GPIO_Setup(PWM6A);
    GPIO_Setup(PWM6B);
    GPIO_Setup(PWM7A);
    GPIO_Setup(PWM7B);
    GPIO_Setup(PWM8A);
    GPIO_Setup(PWM8B);
}

void Init_class::Variables()
{

    INV.counter = 0.0f;


    Meas_alarm_H.temperature = 65.0f;
    Meas_alarm_L.temperature = -100.0f;

    Meas_alarm_H.U_dc = 690.0f;
    Meas_alarm_L.U_dc = -500.0f;

    Meas_alarm_H.I_conv = 160.0f;
    Meas_alarm_L.I_conv = -160.0f;

    Meas_alarm_H.I_grid = 160.0f;
    Meas_alarm_L.I_grid = -160.0f;

    CIC2_calibration.decimation_ratio = 5.0f;
    CIC2_calibration.decimation_counter = 4.0f;
    CIC2_calibration.OSR = 125;
    CIC2_calibration.div_OSR = 1.0f / CIC2_calibration.OSR;
    CIC2_calibration.range_modifier = 2500.0f;
    CIC2_calibration.div_range_modifier = 1.0f / CIC2_calibration.range_modifier;
    CIC2_calibration_input.ptr = &Meas.U_dc_0;
}

const struct GPIO_struct GPIOreg[169] =
{
[SD_SPISIMO_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPISOMI_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPICLK_PIN] = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[SD_SPISTE_PIN] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[EM1D0 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D1 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D2 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D3 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D4 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D5 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D6 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D7 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D8 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D9 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D10] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D11] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D12] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D13] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D14] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D15] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},

[EM1CS2] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1WE ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1OE ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},

[EM1BA1] = {HIGH, MUX3, CPU1_IO, INPUT, ASYNC},
[EM1A0 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A1 ] = {HIGH, MUX0, CPU1_IO, OUTPUT, ASYNC},
[EM1A2 ] = {HIGH, MUX0, CPU1_IO, OUTPUT, ASYNC},
[EM1A3 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A4 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A5 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A6 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A7 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A8 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A9 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A10] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A11] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},

[PWM1A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM1B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM2A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM2B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM3A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM3B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM4A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM4B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM5A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM5B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM6A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM6B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM7A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM7B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM8A] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM8B] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},

[TZ_EN_CPU] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM_SYNC_CPU] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC},

[RELAY_H1_DC_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H2_DC_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H3_DC_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H4_DC_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},

[RELAY_H1_GRID_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H2_GRID_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H3_GRID_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[RELAY_H4_GRID_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},

[PWM_H1_ON_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[PWM_H2_ON_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[PWM_H3_ON_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[PWM_H4_ON_CPU] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},

[LED1_ON_CPU] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED2_ON_CPU] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED3_ON_CPU] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

};

void Init_class::GPIO()
{

    GPIO_Setup(RELAY_H1_DC_CPU);
    GPIO_Setup(RELAY_H2_DC_CPU);
    GPIO_Setup(RELAY_H3_DC_CPU);
    GPIO_Setup(RELAY_H4_DC_CPU);

    GPIO_Setup(RELAY_H1_GRID_CPU);
    GPIO_Setup(RELAY_H2_GRID_CPU);
    GPIO_Setup(RELAY_H3_GRID_CPU);
    GPIO_Setup(RELAY_H4_GRID_CPU);

    GPIO_Setup(PWM_H1_ON_CPU);
    GPIO_Setup(PWM_H2_ON_CPU);
    GPIO_Setup(PWM_H3_ON_CPU);
    GPIO_Setup(PWM_H4_ON_CPU);

    GPIO_Setup(LED1_ON_CPU);
    GPIO_Setup(LED2_ON_CPU);
    GPIO_Setup(LED3_ON_CPU);
}
