;fulltester
DAT:   .data 9,     +17 		,24,-3,-6,-5563,                          0003
STRING: .string      		"abcde  f   g    h"
RE493: inc FF
.define zero =    0
.define number0n3=1

DO:   mov DAT[zero], DAT[number0n3]
          .entry            HELLO
HELLO:   add	 NOTDECLARED	,	 r1
;please ignore this line
.define nUm12321 = 7
LIST: .data 6,zero, number0n3
.define NuMbER        =            -16
ANOtherDAT: .data  198 	,   	199,200,201,202	,	0434, 003
stop
CMpLabel: cmp    FF[number0n3]   ,  WHATISTHAT[nUm12321]
mov 	STRING[nUm12321]	 ,veRyW31rdW0RD987[NuMbER]
mov 	veRyW31rdW0RD987[NuMbER],     STRING[nUm12321]	
						sub  STRING[nUm12321]   ,		 r1
sub  STRING[nUm12321]   	, r2 
					sub R4,r2
inc number0n3
COMpare: cmp 	zero  , number0n3

FF: .data 647, 876
not r2
inc	r7
dec   	r6
;yet another comment

red r1
red 	register2
prn	  r6
cmp 		r1,r2
lea DAT  ,      r1
veRyW31rdW0RD987: .string "just a mumble jumble of l e t t e r s"
;errors:
; Usage of undeclared symbols, in lines: 
; 10 - NOTDECLARED
; 22 - R4
; 17 - WHATISTHAT
; 33 - register2
