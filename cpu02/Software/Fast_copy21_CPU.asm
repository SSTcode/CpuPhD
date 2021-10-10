;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy21_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Fast_copy21_CPUasm, _Fast_copy21_CPUasm
		.endif

        .global _Fast_copy21_CPUasm
        .text

_Fast_copy21_CPUasm:
		MOVL	XAR0, #0x03FC00
		MOVW    DP, #_PieCtrlRegs.PIEIFR11

wait:	TBIT	@_PieCtrlRegs.PIEIFR11, #0
		SBF		wait, NTC
		TCLR	@_PieCtrlRegs.PIEIFR11, #0


        LRETR                           ;Return

;end of function Fast_copy21_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
