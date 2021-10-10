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
		MOVL	XAR0, #0x040000
		IACK  	#0x0001

        LRETR                           ;Return

;end of function Fast_copy12_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
