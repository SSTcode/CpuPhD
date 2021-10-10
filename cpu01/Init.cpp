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

void Init_class::clear_alarms()
{
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

void Init_class::CPUS()
{
    EALLOW;

    DevCfgRegs.CPUSEL0.bit.EPWM11 = 1;

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
    MemCfgRegs.GSxMSEL.bit.MSEL_GS13 = 1;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 1;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS15 = 1;

   EDIS;
}

void Init_class::PWM_sync()
{
    EALLOW;
    CpuSysRegs.PCLKCR3.bit.ECAP1 = 1;

    // Setup APWM mode on CAP1, set period and compare registers
    //
    ECap1Regs.ECCTL2.bit.CAP_APWM = 1;      // Enable APWM mode
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 1;      // Select CTR = PRD event to be the sync-out signal
    ECap1Regs.ECCTL1.bit.FREE_SOFT = 2;
    ECap1Regs.CAP1 = (10000 << 1) - 1;            // Set Period value
    ECap1Regs.CAP4 = (ECap1Regs.CAP1 + 1) >> 1; // 50% duty
    //
    // Start counters
    //
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;

    OutputXbarRegs.OUTPUT3MUX0TO15CFG.bit.MUX0 = 3; // Select ECAP1.OUT on Mux0
    OutputXbarRegs.OUTPUT3MUXENABLE.bit.MUX0 = 1;  // Enable MUX0 for ECAP1.OUT
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
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;
    EDIS;

    PWM_timestamp(&EPwm10Regs);
    PWM_sync();
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

void Init_class::Variables()
{
    Meas_alarm_H.Temp = 65.0f;
    Meas_alarm_L.Temp = 0.0f;

    Meas_alarm_H.U_dc = 690.0f;
    Meas_alarm_L.U_dc = -50.0f;

    CIC2_calibration.decimation_ratio = 5.0f;
    CIC2_calibration.decimation_counter = 4.0f;
    CIC2_calibration.OSR = 125;
    CIC2_calibration.div_OSR = 1.0f / CIC2_calibration.OSR;
    CIC2_calibration.range_modifier = 2500.0f;
    CIC2_calibration.div_range_modifier = 1.0f / CIC2_calibration.range_modifier;
    CIC2_calibration_input.ptr = &Meas.U_dc;
}

const struct GPIO_struct GPIOreg[169] =
{
[SD_SPISIMO_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPISOMI_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPICLK_PIN] = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[SD_SPISTE_PIN] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[PWM_SYNC] = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
};

void Init_class::GPIO()
{
    GPIO_Setup(PWM_SYNC);
}
