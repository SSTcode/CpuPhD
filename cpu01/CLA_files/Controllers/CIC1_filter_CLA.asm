;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	CIC1_filter_CLAasm, _CIC1_filter_CLAasm
		.endif

		.global _CIC1_filter_CLAasm
		.def	__cla_CIC1_filter_CLAasm_sp

SIZEOF_LFRAME	.set	3
LFRAME_MR3		.set	0
LFRAME_CIC_addr .set	2

__cla_CIC1_filter_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_CIC1_filter_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_CIC1_filter_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_CIC1_filter_CLAasm"

		.align 	2

; C prototype:
; void CIC1_filter_CLAasm(volatile struct CIC1_struct *CIC, float input);
; 38/28 instructions without pre-jump

_CIC1_filter_CLAasm:
;		MDEBUGSTOP
		MMOV32		MR1, *MAR0+[CIC1_struct.decimation_counter]
		MMOV32		MR1, *MAR0+[CIC1_struct.decimation_ratio], LEQ
		MADDF32		MR1, MR1, #-1.0f
		MMOV32		*MAR0+[CIC1_struct.decimation_counter], MR1
		MRCNDD		GT							; return call

		MMOV16 		@LFRAME + LFRAME_CIC_addr, MAR0
		MMOV32		MR1, *MAR0+[CIC1_struct.range_modifier]
		MMPYF32		MR0, MR0, MR1
||		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MF32TOI32	MR0, MR0
		MMOV32	  	MR1, *MAR0+[CIC1_struct.integrator]
		MADD32		MR1, MR1, MR0
		MMOV32	  	*MAR0+[CIC1_struct.integrator], MR1

; MR0 = counter + 1
; MR1 = OSR
; MR3 = div_OSR
		MMOV32	  	MR3, *MAR0+[CIC1_struct.div_OSR]
		MMOV32	 	MR2, *MAR0+[CIC1_struct.counter]
		MMOV32	 	MR2, *MAR0+[CIC1_struct.OSR], LEQ
		MADDF32		MR2, MR2, #-1.0f
		MMOV32	  	MR0, *MAR0+[CIC1_struct.div_memory]
		MMPYF32		MR2, MR2, MR3
||		MMOV32 		*MAR0+[CIC1_struct.counter], MR2
		MMPYF32		MR2, MR2, #CIC_upsample1.0f
		MF32TOI32	MR2, MR2

		MCMP32		MR0, MR2
		MMOVZ16		MR1, @LFRAME + LFRAME_CIC_addr
		MADD32		MR1, MR1, MR2
		MADD32		MR1, MR1, MR2
		MRCNDD    	EQ
		MMOV16		MAR1, MR1, #0
		MMOV32	  	*MAR0+[CIC1_struct.div_memory], MR2
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3

; MR0 = x
; MR1 = x
; MR2 = x
; MR3 = div_OSR
		MMOV32	  	MR3, *MAR0+[CIC1_struct.div_OSR]
		MMOV32	  	MR0, *MAR1+[CIC1_struct.decimator_memory]
		MMOV32	  	MR1, *MAR0+[CIC1_struct.integrator]
		MMOV32	  	*MAR1+[CIC1_struct.decimator_memory], MR1
; MR0 = subtractor[0]
; MR1 = integrator[1]
; MR2 = subtractor[1]
; MR3 = div_OSR
		MSUB32		MR1, MR1, MR0
		MI32TOF32 	MR1, MR1
		MRCNDD		UNC							; return call
		MMPYF32		MR1, MR1, MR3
||		MMOV32		MR2, *MAR0+[CIC1_struct.div_range_modifier]
		MMPYF32		MR1, MR1, MR2
||		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3
		MMOV32	  	*MAR0+[CIC1_struct.out], MR1

		.unasg	LFRAME

; end of file
