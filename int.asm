        .ORIG x1200            ; Start of interrupt service routine

        ; Save R0 and R1 to the stack to preserve them
        STW R0, R6, #-1        ; Push R0 onto the stack ; x1200
        ADD R6, R6, #-2         ; x1202
        STW R1, R6, #-1        ; Push R1 onto the stack ; x1204
        ADD R6, R6, #-2         ; x1206

        ; Load the address x4000 into R0
        LEA R0, TIMERADDRESS   ; Load address of x4000 into R0 ; x1208
        LDW R0, R0, #0                  ;x120a

        ; Load the value at memory location x4000 into R1
        LDW R1, R0, #0         ; Load the current value at x4000 into R1 ; 0x120c

        ; Increment the value in R1 by 1
        ADD R1, R1, #1         ; Increment the value ; 0x120e

        ; Store the incremented value back into memory location x4000
        STW R1, R0, #0         ; Store the updated value back into x4000 ; 0x1210

        ; Restore R1 and R0 from the stack
        LDW R1, R6, #0         ; Pop R1 from the stack ; 0x1212
        ADD R6, R6, #2
        LDW R0, R6, #0         ; Pop R0 from the stack
        ADD R6, R6, #2                                  ; 0x1218

        ; Return from interrupt
        RTI                    ; Return from interrupt ; 0x121a

TIMERADDRESS .FILL x4000       ; Memory location holding address x4000
        .END
