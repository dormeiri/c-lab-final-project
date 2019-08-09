;example


.define sz = 2
;with


MAIN: mov r3, LIST[sz]

LOOP: jmp L1

prn #-5

;spaces and comments

mov STR[5], STR[2]
sub r1, r4
cmp r3, #sz
bne END
         

;effectivly should
			

L1: inc K
bne LOOP
       
;output the same 

          		

END: stop
.define len = 4
			
;machine code

STR: .string "abcdef"
LIST: .data 6, -9, len
K: .data 22