; This program displays the binary pattern
; from 0 to 255 (and back to 0) on the LEDs
; interfaced with port 1.

; A 1 in the pattern is represented by the LED on,
; while a 0 in the pattern is represented by the LED off.

; However, logic 0 on a port 1 pin turns on the LED,
; therefore it is necessary to write the inverse of the
; pattern to the LEDs. The easiest way to do this is
; to send the data FFH to 0 (and back to FFH) to the LEDs.

; Since port 1 is initially at FFH all we need to do is
; continuously decrement port 1.

start:
	DEC p1
	JMP start