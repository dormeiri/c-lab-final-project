.entry  LIST
.extern W
.define sz = 2

MAIN:   mov     r+1,r-1
LOOP:   jmp     r8
        prn     rsz
        mov     r0, r7
        sub     r00, r01
        cmp     K, #sz
        bne     W
L1:     inc     L3
.entry  LOOP
        bne     LOOP
END:    stop

.define len = 4

STR:    .string "abcdef"
LIST:   .data   6, -9, len
K:      .data   22
.extern L3