;test_success_1

.define          len=4              

.entry  LIST
.extern W
.define           sz         =    2  
      .define    whatisthat         =    +2  

MAIN:   mov     r3         ,LIST[sz]
LOOP:   jmp     W
        prn     #-5
        mov     STR[5]     ,          STR[2]
        sub     r1,r4
        cmp           K    ,  #sz    
        bne     W
L1:     inc     L3
.entry  LOOP
STR:    .string         "abcdef"
        bne     LOOP
END:    stop


; Some comment

         ; Some comment 2

       LIST:   .data   6           , -9,len

    K:      .data   +22

.extern      L3          