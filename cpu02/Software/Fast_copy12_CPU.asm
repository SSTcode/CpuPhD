;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy12_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Fast_copy12_CPUasm, _Fast_copy12_CPUasm
		.endif

        .global _Fast_copy12_CPUasm
        .text

_Fast_copy12_CPUasm:
		MOVL	XAR4, #0x040000
		MOV32	*(#_Meas.temperature_0A), *--XAR4
		MOV32	*(#_Meas.temperature_0B), *--XAR4
		MOV32	*(#_Meas.temperature_1A), *--XAR4
		MOV32	*(#_Meas.temperature_1B), *--XAR4
		MOV32	*(#_Meas.temperature_2A), *--XAR4
		MOV32	*(#_Meas.temperature_2B), *--XAR4
		MOV32	*(#_Meas.temperature_3A), *--XAR4
		MOV32	*(#_Meas.temperature_3B), *--XAR4
		MOV32	*(#_Meas.I_conv_0), *--XAR4
		MOV32	*(#_Meas.U_dc_0), *--XAR4
		MOV32	*(#_Meas.I_conv_1), *--XAR4
		MOV32	*(#_Meas.U_dc_1), *--XAR4
		MOV32	*(#_Meas.I_conv_2), *--XAR4
		MOV32	*(#_Meas.U_dc_2), *--XAR4
		MOV32	*(#_Meas.I_conv_3), *--XAR4
		MOV32	*(#_Meas.U_dc_3), *--XAR4
		MOV32	*(#_Meas.I_grid), *--XAR4
		MOV32	*(#_Meas.U_grid), *--XAR4
		MOV32	*(#_Meas.I_inv_0), *--XAR4
		MOV32	*(#_Meas.I_inv_1), *--XAR4
		MOV32	*(#_Meas.I_inv_2), *--XAR4
		MOV32	*(#_Meas.I_inv_3), *--XAR4

		IACK  	#0x0001

        LRETR                           ;Return

;end of function Fast_copy12_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
