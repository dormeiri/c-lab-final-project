;fulltester


DAT:   .data 9,     +17 		,word1 ,-6,-5563
DATS:   .data 9,     +17 		,"word2" ,-6,-5563
STRING: .string      		"abcde  f   g    h"

RE493: inc FF

.define zero =    0
.define number0n3=1

DO:   mov DAT[zero], DAT[number0n3]

;SHOULDWARN:          .entry            HELLO
HELLO:   add	 #1	,	 r1
;SHOUldWArn2:		.extern                         GOODBYE
;please ignore this line

as well as THIS ONE

.define nUm12321 = 7


LIST: .data 6,zero, number0n3
.define NuMbER        =            -16

ANOtherDAT: .data  198 	,   	199,200,201,202	,	0434, 003
stop

CMpLabel: cmp    FF[number0n3]   ,  DAT[nUm12321]

mov 	STRING[nUm12321]	 ,veRyW31rdW0RD987[NuMbER]
mov 	veRyW31rdW0RD987[NuMbER],     STRING[nUm12321]	
						sub  STRING[nUm12321]   ,		 r1
sub  STRING[nUm12321]   	, r2 
					sub r4,r2
inc number0n3
COMpare: cmp 	zero  , number0n3

FF: .data 647, 876

not r2, r3 
inc	r7
dec   	r6

;yet another comment

r3: red r1
TooooooooooLoooooooooong1234567: red 	r2
TooooooooooLoooooooooong12345678: red 	r2
prn	  r6
cmp 		r2

not 

lea DAT  ,      r1
veRyW31rdW0RD987: .string "just a mumble jumble of l e t t e r s"
;
;

;errors: 
; undefined label (word1) (line 4)
; non integer when expecting one (line 5)
; undefined line (random string out of context (line 20))
; too many operands: ("not r2, r3") (line 43)
; invalid label (r3:) (line 49)
; invalid label (TooooooooooLoooooooooong123456789) (line 51)
; missing arguments: "cmp r2" (line 53) 
; No arguments when expected ("not") (line 55)
