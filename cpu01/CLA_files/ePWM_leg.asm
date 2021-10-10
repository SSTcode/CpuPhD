;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.if __TI_EABI__
		.asg	ePWM_leg, _ePWM_leg
		.endif

		.global _ePWM_leg
		.def	__cla_ePWM_leg_sp

SIZEOF_LFRAME	.set	2
LFRAME_MR3		.set	0

__cla_ePWM_leg_sp	.usect ".scratchpad:Cla1Prog:_ePWM_leg", SIZEOF_LFRAME, 0, 1
		.asg	 __cla_ePWM_leg_sp, LFRAME

		.sect 	"Cla1Prog:_ePWM_leg"

		.align 	2

; C prototype:
; void ePWM_leg(volatile struct EPWM_REGS *EPwmRegT1T2, float duty);
; argument 1 = *EPwmRegT1T2 : 32-bit T1,T2 transistors PWM register address [MAR0]
; argument 2 = rk : 32-bit floating point duty <0,1> [MR0]
; 15 instructions without pre-jump

_ePWM_leg:
;MR0 = duty
;MR1 = X
;MR2 = X
;MR3 = X
;		MDEBUGSTOP
		MUI16TOF32 	MR1,*MAR0+[63h]

		MMPYF32   	MR0, MR1, MR0
||		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MADDF32		MR1, MR1, #-0.50390625f

		MMOVIZ		MR2, #0

		MMOVXI		MR3, #0x0002
		MMINF32   	MR0, MR1
		MMOV32		MR2, MR3, GT

		MMOVXI		MR3, #0x0001
		MMAXF32   	MR0, #0.50390625f
		MMOV32		MR2, MR3, LT

		MF32TOUI16R MR0, MR0

		MRCNDD		UNC							; return call
		MMOV16    	*MAR0+[6Bh], MR0
		MMOV16    	*MAR0+[49h], MR2
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3

		.unasg	LFRAME

; end of file
