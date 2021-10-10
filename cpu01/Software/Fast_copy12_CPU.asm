;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy12_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Fast_copy12_CPUasm, _Fast_copy12_CPUasm
		.endif

        .global _Fast_copy12_CPUasm
        .sect ".TI.ramfunc"

_Fast_copy12_CPUasm:

		MOVL	XAR0, #_EMIF_mem.read
		MOV32	*(#_EMIF_CLA.temperature_0A), *XAR0++
		MOV32	*(#_EMIF_CLA.temperature_1A), *XAR0++
		MOV32	*(#_EMIF_CLA.temperature_2A), *XAR0++
		MOV32	*(#_EMIF_CLA.temperature_3A), *XAR0++
		MOV32	*(#_EMIF_CLA.I_conv_0), *XAR0++
		MOV32	*(#_EMIF_CLA.I_conv_1), *XAR0++
		MOV32	*(#_EMIF_CLA.I_conv_2), *XAR0++
		MOV32	*(#_EMIF_CLA.I_conv_3), *XAR0++
		MOV32	*(#_EMIF_CLA.I_grid), *XAR0++

		IACK  	#0x0001

		MOVL	XAR4, #0x040000

		MOVW    DP, #_PieCtrlRegs.PIEIFR11
		TCLR	@_PieCtrlRegs.PIEIFR11, #0
wait2:	TBIT	@_PieCtrlRegs.PIEIFR11, #0
		SBF		wait2, NTC
		TCLR	@_PieCtrlRegs.PIEIFR11, #0

		MOV32	*--XAR4, *(#_Meas.temperature_0A)
		MOV32	*--XAR4, *(#_Meas.temperature_0B)
		MOV32	*--XAR4, *(#_Meas.temperature_1A)
		MOV32	*--XAR4, *(#_Meas.temperature_1B)
		MOV32	*--XAR4, *(#_Meas.temperature_2A)
		MOV32	*--XAR4, *(#_Meas.temperature_2B)
		MOV32	*--XAR4, *(#_Meas.temperature_3A)
		MOV32	*--XAR4, *(#_Meas.temperature_3B)
		MOV32	*--XAR4, *(#_Meas.I_conv_0)
		MOV32	*--XAR4, *(#_Meas.U_dc_0)
		MOV32	*--XAR4, *(#_Meas.I_conv_1)
		MOV32	*--XAR4, *(#_Meas.U_dc_1)
		MOV32	*--XAR4, *(#_Meas.I_conv_2)
		MOV32	*--XAR4, *(#_Meas.U_dc_2)
		MOV32	*--XAR4, *(#_Meas.I_conv_3)
		MOV32	*--XAR4, *(#_Meas.U_dc_3)
		MOV32	*--XAR4, *(#_Meas.I_grid)
		MOV32	*--XAR4, *(#_Meas.U_grid)

		MOVW    DP, #_IpcRegs
		TSET	@_IpcRegs.IPCSET, #3

		TSET	@_IpcRegs.IPCCLR, #3

        LRETR                           ;Return

;end of function Fast_copy12_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
