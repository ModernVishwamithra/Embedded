; WAP to add the contents of Internal RAM locations 40H and 41H. Store
; Result at 42H and Carry at 43H

        MOV 43H, #00H ; Initialize Carry as “0”
        MOV A, 40H ; Read first number
        ADD A, 41H ; Add second number
        JNC SKIP ; If no Carry, directly store the Sum
        INC 43H ; Store Carry as “1”
SKIP:   MOV 42H, A ; Store Sum
HERE:   SJMP HERE ; End of program