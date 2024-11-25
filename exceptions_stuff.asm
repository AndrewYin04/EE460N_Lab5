        ; Step 4: Cause desired exceptions

        ; Uncomment one of the following lines based on the exception you want to test:

        ; Protection Exception (Store sum at x0000)
        ; LEA   R1, PROTEXCEPTADDR
        ; LDW R1, R1, #0
        ; STW   R3, R1, #0

        ; Unaligned Access Exception (Store sum at xC017)
        ; LEA   R1, UNALIGNEDADDR
        ; LDW R1, R1, #0
        ; STW   R3, R1, #0

        ; Unknown Opcode Exception (Invalid opcode)
        ; .FILL xA000             ; An invalid opcode (1010)
        ; .FILL xB000           ; Invalid opcode (0b1011)