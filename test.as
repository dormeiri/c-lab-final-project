.define x=7

#Declared after usage
mov C, B[x]

.extern C
.entry B
B: .data 1,-2