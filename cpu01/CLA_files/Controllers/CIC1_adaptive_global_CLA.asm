;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	CIC1_adaptive_global_CLAasm, _CIC1_adaptive_global_CLAasm
		.endif

		.global _CIC1_adaptive_global_CLAasm
		.def	__cla_CIC1_adaptive_global_CLAasm_sp

SIZEOF_LFRAME	.set	2
LFRAME_MR3		.set	0

__cla_CIC1_adaptive_global_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_CIC1_adaptive_global_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_CIC1_adaptive_global_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_CIC1_adaptive_global_CLAasm"

		.align 	2

; C prototype:
; void CIC1_adaptive_global_CLAasm(volatile struct CIC1_adaptive_global_struct *CIC_global, float frequency);
; 41+35 = 77
_CIC1_adaptive_global_CLAasm:
;		MDEBUGSTOP
		MMOV32	  	MR1, *MAR0+[CIC1_adaptive_global_struct.Ts]
		MMPYF32   	MR0, MR1, MR0
||		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3

		MEINVF32  	MR3, MR0
		MMPYF32   	MR2, MR0, MR3
		MSUBF32   	MR2, #2.0f, MR2
		MMPYF32   	MR3, MR2, MR3
		MMPYF32   	MR2, MR3, MR0
		MSUBF32   	MR2, #2.0f, MR2
		MMPYF32   	MR0, MR2, MR3
||		MMOV32	  	MR1, *MAR0+[CIC1_adaptive_global_struct.OSR_adaptive]

		MSUBF32		MR1, MR1, MR0
		MABSF32		MR1, MR1

		MCMPF32		MR1, #0.75f
		MNOP
		MADDF32		MR0, MR0, #0.5f
		MF32TOI32	MR0, MR0
		MBCNDD 		_Skip, LT
		MI32TOF32	MR0, MR0
		MEINVF32  	MR3, MR0
		MMPYF32   	MR2, MR0, MR3

		MMOV32	  	MR1, *MAR0+[CIC1_adaptive_global_struct.change_timer]
		MSUBF32   	MR2, #2.0f, MR2
		MMPYF32   	MR3, MR2, MR3
		MMPYF32   	MR2, MR3, MR0
		MBCNDD 		_Skip, GEQ
		MSUBF32   	MR2, #2.0f, MR2
		MMPYF32   	MR3, MR2, MR3
		MMOVIZ		MR1, #0.5625f


		MMOV32		*MAR0+[CIC1_adaptive_global_struct.OSR_adaptive], MR0
		MMOV32		*MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive], MR3
		MMOV32		*MAR0+[CIC1_adaptive_global_struct.change_timer], MR1

_Skip:
		MMOVIZ		MR1, #1.0f
		MMOVIZ		MR0, #0.0f
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.change_timer]
		MCMPF32		MR2, #0.28125f
		MMOV32		MR0, MR1, GEQ
		MMOV32	  	MR3, *MAR0+[CIC1_adaptive_global_struct.Ts]
		MSUBF32		MR2, MR2, MR3
||		MMOV32		*MAR0+[CIC1_adaptive_global_struct.select_output], MR0
		MBCNDD 		_Skip2, GEQ
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.change_timer], MR2
		MMOV32	  	MR0, *MAR0+[CIC1_adaptive_global_struct.OSR_adaptive]
		MMOV32	  	MR1, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive]

		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.OSR_adaptive + 2], MR0
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive + 2], MR1

_Skip2:

		MMOVIZ		MR1, #1.0f
		MMOV32	  	MR3, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive]
		MMOV32	  	MR0, *MAR0+[CIC1_adaptive_global_struct.OSR_adaptive]
		MMOV32	 	MR2, *MAR0+[CIC1_adaptive_global_struct.counter]
		MMOV32	 	MR2, MR0, LEQ
		MMINF32		MR2, MR0
		MADDF32		MR2, MR2, #-1.0f
		MMPYF32		MR3, MR2, MR3
||		MMOV32 		*MAR0+[CIC1_adaptive_global_struct.counter], MR2
		MMPYF32		MR3, MR3, #CIC_upsample2.0f
		MF32TOI32	MR3, MR3
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory]
		MADD32		MR3, MR3, MR3
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.div_memory], MR3
		MMOVIZ		MR0, #0.0f
		MCMP32		MR2, MR3
		MMOV32	  	MR0, MR1, NEQ
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.cycle_enable], MR0


		MMOV32	  	MR3, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive + 2]
		MMOV32	 	MR0, *MAR0+[CIC1_adaptive_global_struct.OSR_adaptive + 2]
		MMOV32	 	MR2, *MAR0+[CIC1_adaptive_global_struct.counter + 2]
		MMOV32	 	MR2, MR0, LEQ
		MMINF32		MR2, MR0
		MADDF32		MR2, MR2, #-1.0f
		MMPYF32		MR3, MR2, MR3
||		MMOV32 		*MAR0+[CIC1_adaptive_global_struct.counter + 2], MR2
		MMPYF32		MR3, MR3, #CIC_upsample2.0f
		MF32TOI32	MR3, MR3
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory + 2]
		MADD32		MR3, MR3, MR3
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.div_memory + 2], MR3
		MMOVIZ		MR0, #0.0f
		MCMP32		MR2, MR3
		MRCNDD    	UNC
		MMOV32	  	MR0, MR1, NEQ
		MMOV32	  	*MAR0+[CIC1_adaptive_global_struct.cycle_enable + 2], MR0
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3
		.unasg	LFRAME

; end of file
