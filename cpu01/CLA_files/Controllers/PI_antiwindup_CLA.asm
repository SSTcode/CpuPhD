;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	PI_antiwindup_CLAasm, _PI_antiwindup_CLAasm
		.endif

		.global _PI_antiwindup_CLAasm
		.def	__cla_PI_antiwindup_CLAasm_sp

SIZEOF_LFRAME	.set	4
LFRAME_MR3		.set	0
LFRAME_int_last .set	2

__cla_PI_antiwindup_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_PI_antiwindup_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_PI_antiwindup_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_PI_antiwindup_CLAasm"

		.align 	2

; C prototype:
; void CIC_filter2_asm(volatile struct CIC_struct *PI, float error);
; 26 instructions without pre-jump

_PI_antiwindup_CLAasm:
;		MDEBUGSTOP
		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MMOV32	  	MR1, *MAR0+[PI_struct.Kp]
		MMPYF32		MR0, MR0, MR1
||		MMOV32	  	MR1, *MAR0+[PI_struct.Ts_Ti]
		MMPYF32		MR1, MR0, MR1
||		MMOV32	  	MR3, *MAR0+[PI_struct.integrator]
		MMOV32	  	@LFRAME + LFRAME_int_last, MR3
		MADDF32		MR3, MR3, MR1
||		MMOV32	  	MR2, *MAR0+[PI_struct.lim_H]
		MADDF32		MR1, MR3, MR0
||		MMOV32	  	*MAR0+[PI_struct.proportional], MR0
;MR0 proportional
;MR1 out
;MR2 lim_H
;MR3 integrator
		MMOV32		MR0, MR3
		MMINF32		MR1, MR2
		MMOV32		MR0, @LFRAME + LFRAME_int_last, GT
		MCMPF32		MR3, MR2
		MMOV32		MR3, MR0, LT
		MCMPF32		MR3, MR0
		MMOV32		MR3, MR0, GT

		MMOV32	  	MR2, *MAR0+[PI_struct.lim_L]
;MR0 integrator_last
;MR1 out
;MR2 lim_L
;MR3 integrator
		MMOV32		MR0, MR3
		MMAXF32		MR1, MR2
		MMOV32		MR0, @LFRAME + LFRAME_int_last, LT
		MCMPF32		MR3, MR2
		MMOV32		MR3, MR0, GT
		MCMPF32		MR3, MR0
		MMOV32		MR3, MR0, LT

		MRCNDD		UNC							; return call
		MMOV32	  	*MAR0+[PI_struct.out], MR1
		MMOV32	  	*MAR0+[PI_struct.integrator], MR3
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3

		.unasg	LFRAME

; end of file
