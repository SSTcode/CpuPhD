/*
 * Hardware.c
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <stdafx.h>
#include "HWIs.h"
#include <Init.h>
#include <math.h>
#include <string.h>


class Init_class Init;

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
    memset(&Meas, 0, sizeof(Meas));
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
    CpuSysRegs.PCLKCR2.bit.EPWM11 = 1;
    EDIS;

    PWM_timestamp(&EPwm11Regs);
}
