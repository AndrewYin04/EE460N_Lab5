        .ORIG x3000

        LEA   R0, INITADDR       ; Load address of x4000 into R0
        LDW   R0, R0, #0
        AND R1, R1, #0
        ADD   R1, R1, #1          ; Set R1 to 1
        STW   R1, R0, #0          ; Store 1 at x4000

        LEA   R2, DATASTART      ; Load base address xC000 into R2
        LDW R2, R2, #0
        AND   R3, R3, #0          ; Clear R3 to hold the sum
        LEA    R4, COUNT           ; Load count (20) into R4
        LDB R4, R4, #0

SUMLOOP LDB   R5, R2, #0          ; Load the byte from xC000 + offset into R5 ; x3014
        ADD   R3, R3, R5          ; Add byte value to the sum in R3            ; x3016
        ADD   R2, R2, #1          ; Move to the next byte ; x3018
        ADD   R4, R4, #-1         ; Decrement the count ; x301a
        BRP   SUMLOOP            ; Repeat until all 20 bytes are processed ; x301c

        LEA   R1, SUMADDR        ; Load address xC014 into R1 ; x301e
        LDW R1, R1, #0              ; x3020
        STW   R3, R1, #0          ; Store the sum at xC014 ; x3022

        ; Step 4: Cause desired exceptions

        ; Uncomment one of the following lines based on the exception you want to test:

        ; Protection Exception (Store sum at x0000)
        ; LEA   R1, PROTEXCEPTADDR ; x3024
        ; LDW R1, R1, #0          ; x3026
        ; STW   R3, R1, #0        ; x3028

        ; Unaligned Access Exception (Store sum at xC017)
        LEA   R1, UNALIGNEDADDR
        LDW R1, R1, #0
        STW   R3, R1, #0

        ; Unknown Opcode Exception (Invalid opcode)
         ;.FILL xA000             ; An invalid opcode (1010)
         ;.FILL xB000           ; Invalid opcode (0b1011)

        HALT                       ; Stop program execution

INITADDR       .FILL x4000         ; Address of x4000
DATASTART      .FILL xC000         ; Starting address of data bytes
COUNT           .FILL #20           ; Number of bytes to sum
SUMADDR        .FILL xC014         ; Address to store the sum
PROTEXCEPTADDR .FILL x0000        ; Address to cause protection exception
UNALIGNEDADDR  .FILL xC017         ; Address to cause unaligned access exception

        .END
