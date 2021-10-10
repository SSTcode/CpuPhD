;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	Filter1_calc_CLAasm, _Filter1_calc_CLAasm
		.endif

		.global _Filter1_calc_CLAasm
		.def	__cla_Filter1_calc_CLAasm_sp

SIZEOF_LFRAME	.set	2
LFRAME_MR3		.set	0

__cla_Filter1_calc_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_Filter1_calc_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_Filter1_calc_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_Filter1_calc_CLAasm"

		.align 	2

; C prototype:
; void Filter1_calc_CLAasm(struct Filter1_struct *Filter1, float input);
; 9 instructions without pre-jump

_Filter1_calc_CLAasm:
;		MDEBUGSTOP
		MMOV32	  	MR2, *MAR0+[Filter1_struct.out]
		MSUBF32		MR0, MR0, MR2
||		MMOV32	  	MR1, *MAR0+[Filter1_struct.Ts_Ti]
		MMPYF32		MR0, MR0, MR1
||		MMOV32	  	MR1, *MAR0+[Filter1_struct.Kahan]
		MSUBF32		MR0, MR0, MR1
		MADDF32		MR1, MR0, MR2
;MR0 y
;MR1 out
;MR2 integrator_last
;MR3

		MRCNDD		UNC							; return call
		MSUBF32		MR2, MR1, MR2
||		MMOV32	  	*MAR0+[Filter1_struct.out], MR1
		MSUBF32		MR2, MR2, MR0
		MMOV32	  	*MAR0+[Filter1_struct.Kahan], MR2

		.unasg	LFRAME

; end of file
