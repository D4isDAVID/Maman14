; booktest without externals (should have .ent file but not .ext)

.entry LENGTH
MAIN:   mov r3, LENGTH
LOOP:   jmp L1(#-1,r6)
        mcr m1
                sub r1,r4
                bne L3
        endmcr
        prn #-5
        bne W(r4,r5)
        m1
L1:     inc K
.entry LOOP
        bne LOOP(K,W)
END:    stop
STR:    .string "abcdef"
LENGTH: .data   6,-9,15
K:      .data   22
W:      .data   1
L3:     .data   2
