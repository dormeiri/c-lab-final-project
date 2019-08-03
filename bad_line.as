xxxxxxxxxxxxxx
yyyyyyyy
.entry  LIST
z
.extern W
.define sz = 2

123
MAIN:   mov     r3,LIST[sz]
                .
LOOP:   jmp     W
        prn     #-5
        mov     STR[5], STR[2]
                .what
        sub     r1, r3
        cmp     K, #sz
        bne     W
    #fs
L1:     inc     L3
    r3
.entry  LOOP
        bne     LOOP
             jmp
END:    stop
[]
[4]
a[3]
.define len = 4

STR:    .string "abcdef"
LIST:   .data   6, -9, len
K:      .data   22
.extern L3



J