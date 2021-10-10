;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	PI_antiwindup_fast_ff_CLAasm, _PI_antiwindup_fast_ff_CLAasm
		.endif

		.global _PI_antiwindup_fast_ff_CLAasm
		.def	__cla_PI_antiwindup_fast_ff_CLAasm_sp

SIZEOF_LFRAME	.set	2
LFRAME_MR3		.set	0

__cla_PI_antiwindup_fast_ff_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_PI_antiwindup_fast_ff_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_PI_antiwindup_fast_ff_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_PI_antiwindup_fast_ff_CLAasm"

		.align 	2

; C prototype:
; void PI_antiwindup_fast_ff_CLAasm(volatile struct PI_struct *PI, float error, float feedforward);
; 17 instructions without pre-jump

_PI_antiwindup_fast_ff_CLAasm:
;		MDEBUGSTOP
		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MMOV32	  	MR3, *MAR0+[PI_struct.Kp]
		MMPYF32		MR0, MR0, MR3
||		MMOV32	  	MR3, *MAR0+[PI_struct.Ts_Ti]
		MMPYF32		MR2, MR0, MR3
||		MMOV32	  	MR3, *MAR0+[PI_struct.integrator]
		MADDF32		MR3, MR3, MR2
||		MMOV32	  	MR2, *MAR0+[PI_struct.lim_H]
		MADDF32		MR0, MR1, MR0
||		MMOV32	  	*MAR0+[PI_struct.proportional], MR0
		MADDF32		MR1, MR3, MR0
;MR0 proportional+feedforward
;MR1 out
;MR2 lim_H
;MR3 integrator
		MMINF32		MR1, MR2
		MSUBF32		MR2, MR2, MR0
		MMOV32		MR3, MR2, GT
;MR0 proportional
;MR1 out
;MR2 lim_L + proportional
;MR3 integrator
		MMOV32	  	MR2, *MAR0+[PI_struct.lim_L]
		MMAXF32		MR1, MR2
		MSUBF32		MR2, MR2, MR0
||		MMOV32	  	*MAR0+[PI_struct.out], MR1

		MRCNDD		UNC							; return call
		MMOV32		MR3, MR2, LT
		MMOV32	  	*MAR0+[PI_struct.integrator], MR3
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3

		.unasg	LFRAME

; end of file
