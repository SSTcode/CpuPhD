
		.cdecls   C,LIST,"stdafx.h"

disable_watchdog	.set	1		; set to 1 to disable watchdog timer

		.ref 	_c_int00			; C boot routine [boot.obj] in rts2800_ml.lib
	    .global _code_start			; long branch to pre-boot: place at code entry point in linnker command file

;-------------------------------------------------------------------------------------------

    	.sect "codestart"
code_start:
		LB		preboot0			; long branch to pre-boot

;-------------------------------------------------------------------------------------------

		.text
preboot0:
		; disable watchdog timer
		.if (disable_watchdog)
    	SETC 	OBJMODE         		; set OBJMODE for 28x object code
    	EALLOW              			; enable EALLOW protected register access
    	MOVW 	DP, #_WdRegs.WDCR		; load data page
	    MOV 	@_WdRegs.WDCR, #0x0068  	; set WDDIS bit in WDCR
    	EDIS                			; disable EALLOW protected register access
		.endif

    	EALLOW              			; enable EALLOW protected register access

	    MOV 	AH, #0x0000
	    MOV 	AL, #0x000F
    	MOVW 	DP, #_MemCfgRegs.DxINIT
	    MOVL 	@_MemCfgRegs.DxINIT, ACC
DxWait: CMP 	AL, @_MemCfgRegs.DxINITDONE
		SBF		DxWait, NEQ

	    MOV 	AL, #0x003F
    	MOVW 	DP, #_MemCfgRegs.LSxINIT
	    MOVL 	@_MemCfgRegs.LSxINIT, ACC
LSxWait:CMP 	AL, @_MemCfgRegs.LSxINITDONE
		SBF		LSxWait, NEQ

		EDIS
    	LB 		stack_fill

;-------------------------------------------------------------------------------------------


***********************************************************************
* Function: stack_fill
*
* Description: Fills the entire C-stack with a value
***********************************************************************
FILL_VALUE    .set    0xDEAD
    .global __STACK_END, __STACK_SIZE
	.text
stack_fill:
    MOVL	XAR0, #(__STACK_END - __STACK_SIZE)			; XAR0 = addr of start of stack
	MOV		AL, #(__STACK_SIZE - 1)						; AL = (stack size - 1) because RPT is N-1
	MOV		AH, #FILL_VALUE								; AH = fill value

	RPT		AL
 || MOV		*XAR0++, AH									; fill the stack

    LB 		_c_int00             ;Branch to start of boot.asm in RTS library

;end stack_fill
***********************************************************************

		.end
	
; end of file
