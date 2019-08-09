.entry  LIST
.extern W
.define sz = 2

r0:   mov     r3,LIST[sz]
R14:   jmp     W
        prn     #-5
        mov     STR[5], STR[2]
        sub     r1, r3
        cmp     K, #sz
        bne     W
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA:     inc     L3
.entry  LOOP
        bne     LOOP
END:    stop

.define len = 4

STR:    .string "abcdef"
LIST:   .data   6, -9, len
K:      .data   22
.extern L3