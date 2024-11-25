.ORIG x3000
LEA r0, location
LDB R1, R0, #0
LDB R2, R0, #1
LDW R3, R0, #0
STB R1, R0, #2
STW R3, R0, #-1
HALT
location .FILL x3020
.END
