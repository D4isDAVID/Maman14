; this file contains every possible error in the first phase

INVALID NAME: mov #0, r0
THISLABELNAMEISWAYWAYWAYTOOLONG: mov #0, r0

; this label is a pre-defined symbol
mov: mov #0, r0

; this label is defined twice
DUPLICATELABEL0: mov #0, r0
DUPLICATELABEL0: mov #0, r0

; this label is defined once but made external later
DUPLICATELABEL1: mov #0, r0
.extern DUPLICATELABEL1

; this label is made external twice
.extern DUPLICATELABEL2
.extern DUPLICATELABEL2


