;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; extern float Resonant_calc_CLAasm(volatile struct Resonant_struct *Resonant, float error);
; argument 1 = *Resonant : structure of internal variables [MAR0]
; argument 2 = error : input error [MR0]
; argument 3 = gain : input controller gain [MR1]
; 18 instructions without pre-jump

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	Resonant_calc_CLAasm, _Resonant_calc_CLAasm
		.endif

		.global _Resonant_calc_CLAasm
		.def	__cla_Resonant_calc_CLAasm_sp

SIZEOF_LFRAME	.set	2
LFRAME_MR3		.set	0

__cla_Resonant_calc_CLAasm_sp	.usect ".scratchpad:Cla1Prog:_Resonant_calc_CLAasm", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_Resonant_calc_CLAasm_sp, LFRAME

		.sect 	"Cla1Prog:_Resonant_calc_CLAasm"

		.align 	2

_Resonant_calc_CLAasm:
;        MDEBUGSTOP
        MMOV16      MAR1, *MAR0+[Resonant_struct.trigonometric]
        MMOV32      MR1, *MAR0+[Resonant_struct.gain]

        MMPYF32     MR0, MR0, MR1
||      MMOV32      MR1, *MAR0+[Resonant_struct.x1]
        MSUBF32     MR2, MR0, MR1
||      MMOV32      @LFRAME + LFRAME_MR3, MR3   ; save MR3
        MADDF32     MR3, MR0, MR1
||      MMOV32      MR1, *MAR1+[trigonometric_struct.cosine]
        MMPYF32     MR3, MR3, MR1
||      MMOV32      MR1, *MAR1+[trigonometric_struct.sine]
        MSUBF32     MR3, MR3, MR0
||      MMOV32      MR0, *MAR0+[Resonant_struct.x0]
        MMPYF32     MR2, MR2, MR1
;MR0 x0(-1)
;MR1 sine
;MR2 sine * (error - x1(-1))
;MR3 cosine * (error + x1(-1)) - error
        MMOV16      MAR1, *MAR0+[Resonant_struct.trigonometric_comp]
        MMPYF32     MR1, MR1, MR0
        MADDF32     MR3, MR1, MR3
||      MMOV32      MR1, *MAR1+[trigonometric_struct.cosine]
;MR0 x0(-1)
;MR1 cosine
;MR2 sine * (error - PR->x1)
;MR3 x1(0)
        MMPYF32     MR0, MR1, MR0
||      MMOV32      *MAR0+[Resonant_struct.x1], MR3
        MADDF32     MR2, MR2, MR0
||      MMOV32      MR0, *MAR1+[trigonometric_struct.sine]
;MR0 sine_comp
;MR1 cosine
;MR2 x0(0)
;MR3 x1(0)
        MMPYF32     MR3, MR3, MR0
||      MMOV32      MR1, *MAR1+[trigonometric_struct.cosine]

		MRCNDD		UNC							; return call
        MMPYF32     MR2, MR2, MR1
||      MMOV32      *MAR0+[Resonant_struct.x0], MR2
        MSUBF32     MR0, MR2, MR3
||		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3
        MMOV32      *MAR0+[Resonant_struct.y0], MR0

		.unasg	LFRAME

; end of file
