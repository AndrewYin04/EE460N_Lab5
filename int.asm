.ORIG x1200

; basically just load page table starting address into a register first
; then have a for loop that loops through all 127 PTEs
; in each iteration of the loop, increment the original register with the address by 2
; AND each PTE with ~(1)
; store it back

; Save R0 and R1 to the stack to preserve them
STW R0, R6, #-1        ; Push R0 onto the stack ; x1200
ADD R6, R6, #-2         ; x1202
STW R1, R6, #-1        ; Push R1 onto the stack ; x1204
ADD R6, R6, #-2         ; x1206
STW R2, R6, #-1
ADD R6, R6, #-2
STW R3, R6, #-1
ADD R6, R6, #-2

LEA R0, PAGETABLE
LDW R0, R0, #0 ; load R0 with the VA of the PT
LEA R1, NUMOFFSET
LDW R1, R0, #0 ; load R1 with the number of PTE to loop through

; make R3 = 0b111110
AND R3, R3, #0
ADD R3, R3, #1
NOT R3, R3

LOOP    ADD R1, R1, #0
        BRZ END
        ADD R1, R1, #-1
        LDW R2, R0, #0 ; load PTE into R2
        AND R2, R2, R3 ; clear bit 0 of the PTE (the reference bit)
        STW R2, R0, #0 ; put modified PTE back to the page table
        ADD R0, R0, #2 ; increment to the next PTE in the page table
        BR LOOP

; Restore R1 and R0 from the stack
END LDW R3, R6, #0
ADD R6, R6, #2
LDW R2, R6, #0
ADD R6, R6, #2
LDW R1, R6, #0         ; Pop R1 from the stack ; 0x1212
ADD R6, R6, #2
LDW R0, R6, #0         ; Pop R0 from the stack
ADD R6, R6, #2                                  ; 0x1218

RTI

PAGETABLE .FILL x1000 ; b0001000000000000 ; VA of page table, which is at page 8
NUMOFFSET .FILL x080 ; decimal 128

.END