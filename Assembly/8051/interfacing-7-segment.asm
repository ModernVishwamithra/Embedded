
START:	SETB P3.3			; |
	SETB P3.4			; | enable display 3
	MOV P1, #11111001B	; put pattern for 1 on display
	CALL DELAY
	CLR P3.3			; enable display 2
	MOV P1, #10100100B	; put pattern for 2 on display
	CALL DELAY
	CLR P3.4			; |
	SETB P3.3			; | enable display 1
	MOV P1, #10110000B	; put pattern for 3 on display
	CALL DELAY
	CLR P3.3			; enable display 0
	MOV P1, #10011001B	; put pattern for 4 on display
	CALL DELAY
	JMP START			; jump back to start

; a crude delay
DELAY: MOV R0, #20
			DJNZ R0,DELAY
			RET
