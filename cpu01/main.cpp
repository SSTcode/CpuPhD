
/*
 *  ======== main.c ========
 */
#include "stdafx.h"
#include "Init.h"
#include "HWIs.h"
#include "State.h"
#include "ff.h"
#include "diskio.h"

FATFS fs;           /* Filesystem object */

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void NMI_INT()
{
    ESTOP0;
}

void main()
{
    EALLOW;
    *(Uint32 *)0xD00 = 0x0B5A;
    EDIS;

    DINT;

    InitFlash();
    InitSysPll(XTAL_OSC, IMULT_40, FMULT_0, PLLCLK_BY_2);

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    EALLOW;
    InputXbarRegs.INPUT5SELECT = PWM_SYNC;
    XintRegs.XINT2CR.bit.POLARITY = 1;
    XintRegs.XINT2CR.bit.ENABLE = 1;
    PieVectTable.XINT2_INT = &HWI_func;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;
    IER |= M_INT1;

    EALLOW;
    PieVectTable.NMI_INT = &NMI_INT;
    EDIS;

    Init.CPUS();

    Init.GPIO();

    Init.CLA();

    Init.PWMs();

//    FatFS_time.second_2 = 5;
//    FatFS_time.minute = 10;
//    FatFS_time.hour = 10;
//    FatFS_time.day = 10;
//    FatFS_time.month = 10;
//    FatFS_time.year = 10 + 20;
//
//    f_mount(&fs, "", 1);

    Machine.state = Machine_init;
    while(1)
    {
        SMachine_Main();
        SMachine_Background();

        static Uint64 benchmark_timer;
        static float benchmark;
        benchmark = (float)(ReadIpcTimer() - benchmark_timer)*(1.0f/200000000.0f);
        benchmark_timer = ReadIpcTimer();
    }
}
