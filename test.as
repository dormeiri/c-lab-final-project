.define x=5
.extern C
.entry B
B: .data 1,-2
mov C, B[x]