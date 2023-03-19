; this file contains various operations with macros

mcr empty
endmcr
empty

mcr newlines



endmcr
newlines

mcr label
SOMELABEL:
endmcr
label .string "hello :)"

mcr zero
#0
endmcr
mov zero , r0

mcr register
r0
endmcr
mov #0, register
