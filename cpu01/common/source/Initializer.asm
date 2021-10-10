
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
	    MOV 	AL, #0x0001
	    MOV 	AH, #0xA5A5
    	MOVW 	DP, #_DevCfgRegs.CPU2RESCTL
	    MOVL 	@_DevCfgRegs.CPU2RESCTL, ACC

		MOV 	AL, #0xFFFF
		RPT		AL
||		NOP

	    MOV 	AL, #0x0000
	    MOVL 	@_DevCfgRegs.CPU2RESCTL, ACC


	    MOV 	AH, #0x0000
    	MOVW 	DP, #_MemCfgRegs.GSxMSEL
	    MOVL 	@_MemCfgRegs.GSxMSEL, ACC

		MOV 	AL, #0xFFFF
		RPT		AL
||		NOP

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

	    MOV 	AL, #0xFFFF
    	MOVW 	DP, #_MemCfgRegs.GSxINIT
	    MOVL 	@_MemCfgRegs.GSxINIT, ACC
GSxWait:CMP 	AL, @_MemCfgRegs.GSxINITDONE
		SBF		GSxWait, NEQ

	    MOV 	AL, #0x0007
    	MOVW 	DP, #_MemCfgRegs.MSGxINIT
	    MOVL 	@_MemCfgRegs.MSGxINIT, ACC
MSxWait:CMP 	AL, @_MemCfgRegs.MSGxINITDONE
		SBF		MSxWait, NEQ

	    MOV 	AH, #0x0000
	    MOV 	AL, #0x0000
    	MOVW 	DP, #_DevCfgRegs.CPUSEL0
	    MOVL 	@_DevCfgRegs.CPUSEL0, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL1, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL2, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL4, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL5, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL6, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL7, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL8, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL9, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL11, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL12, ACC
	    MOVL 	@_DevCfgRegs.CPUSEL14, ACC

	    MOV 	AH, #0x0000
    	MOVW 	DP, #_DevCfgRegs.SOFTPRES0
	    MOV 	AL, #0x0005
	    MOVL 	@_DevCfgRegs.SOFTPRES0, ACC
	    MOV 	AL, #0x0003
	    MOVL 	@_DevCfgRegs.SOFTPRES1, ACC
	    MOV 	AL, #0x03FF
	    MOVL 	@_DevCfgRegs.SOFTPRES2, ACC
	    MOV 	AL, #0x003F
	    MOVL 	@_DevCfgRegs.SOFTPRES3, ACC
	    MOV 	AL, #0x0007
	    MOVL 	@_DevCfgRegs.SOFTPRES4, ACC
	    MOV 	AL, #0x0003
	    MOVL 	@_DevCfgRegs.SOFTPRES6, ACC
	    MOV 	AL, #0x000F
	    MOVL 	@_DevCfgRegs.SOFTPRES7, ACC
	    MOV 	AL, #0x0007
	    MOVL 	@_DevCfgRegs.SOFTPRES8, ACC
	    MOV 	AL, #0x0003
	    MOVL 	@_DevCfgRegs.SOFTPRES9, ACC
	    MOV 	AL, #0x0003
	    MOVL 	@_DevCfgRegs.SOFTPRES11, ACC
	    MOV 	AL, #0x000F
	    MOVL 	@_DevCfgRegs.SOFTPRES13, ACC
	    MOV 	AL, #0x00FF
	    MOVL 	@_DevCfgRegs.SOFTPRES14, ACC
	    MOV 	AH, #0x0007
	    MOV 	AL, #0x0000
	    MOVL 	@_DevCfgRegs.SOFTPRES16, ACC

		MOV 	AL, #0xFFFF
		RPT		AL
||		NOP

	    MOV 	AH, #0x0000
	    MOV 	AL, #0x0000
	    MOVL 	@_DevCfgRegs.SOFTPRES0, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES1, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES2, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES3, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES4, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES6, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES7, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES8, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES9, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES11, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES13, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES14, ACC
	    MOVL 	@_DevCfgRegs.SOFTPRES16, ACC

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
