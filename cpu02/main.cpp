
/*
 *  ======== main.c ========
 */
#include "stdafx.h"
#include "Init.h"
#include "HWIs.h"

void main()
{
    InitFlash();

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    EALLOW;
    PieVectTable.IPC3_INT = &HWI_func;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx16 = 1;
    IER |= M_INT1;

    Init.PWMs();

    Init.CLA();

    Init.Variables();

    EALLOW;
    Cla1Regs.MIER.bit.INT1 = 1;
    EDIS;

    EINT;

    while(1)
    {
    }
}
