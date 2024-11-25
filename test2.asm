   .ORIG x3000
   ADD R0, R0, #-2
loop ADD R1, R0, R1
AND R2, R1, #-6
BRn loop
   HALT
   .END
