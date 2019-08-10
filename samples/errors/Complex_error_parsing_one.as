STRING: .string      		"abcde  f   g    h" ;comment

RE493: inc FF

.define zero =    0
.define number0n3=1

DO:   movv DAT[zero], DAT[number0n3]

;SHOULDWARN:          .entry            HELLO
HELLO:   add	 #1	,	 r1
;SHOUldWArn2:		.extern                         GOODBYE
;please ignore this line

;as well as THIS ONE

.define nUm12321 = 7


LIST: .data 6,zero, number0n3
.define NuMbER        =            -16

ANOtherDAT: .data  198 	,   	199,200,201,202		0434, 003
stop

CMpLabel cmp    FF[number0n3]   ,  DAT[nUm12321]
CMpLabel: comp    FF[number0n3]   ,  DAT[nUm12321]

mov 	STRING[nUm12321]	 ,veRyW31rdW0RD987[NuMbER]
mov 	veRyW31rdW0RD987[NuMbER],     STRING[nUm12321]	
						sub  STRING[nUm12321]   ,		 r1
sub  STRING[nUm12321]   	, r2 
					sub r4,r2
inc number0n3
COMpare: cmp 	zero  , number0n3

FF: .data 647, 876,             				

not r2
inc	r7
dec   	r6

ANOtherDAT: .data 15, 24, 55

;yet another comment

red r1
red 	r2
prn	  r6
cmp 		r1,r2



lea DAT  ,      r1
veRyW31rdW0RD987: .string "just a mumble jumble of l e t t e r s"

;errors:	
;			string invalid comment after EOL (line 1)
;			wrong operation name, movv instead of mov (line 8)
;			missing ',' in "ANOtherDAT:..." Line (line 23)
;			wrong operation instead of label due to missing ':' (line 26)
;			wrong operation name, comp instead of cmp (line 27)
;			Extraneous text after command (in FF: .data line 37)
;			ANOtherDAT declared twice (line 43)
