    .segment "CHARMEM"
    .export _CHARMEM
_CHARMEM:
    .res    128*8
    ; 128
    ; 6 top left
    .incbin "images/6.bitmap"
    ; 129
    ; 7 top left
    .incbin "images/7.bitmap"
    ; 130
    ; top
    .byte   $ff, $00, $00, $00, $00, $00, $00, $00
    ; 131
    ; right
    .byte   $01, $01, $01, $01, $01, $01, $01, $01
    ; 132
    ; bottom right
    .byte   $01, $01, $01, $01, $01, $01, $01, $fe
    ; 133
    ; bottom
    .byte   $00, $00, $00, $00, $00, $00, $00, $ff
    ; 134
    ; bottom left
    .byte   $80, $80, $80, $80, $80, $80, $80, $7f
    ; 135
    ; top right
    .byte   $fe, $01, $01, $01, $01, $01, $01, $01
    ; 136
    ; left
    .byte   $80, $80, $80, $80, $80, $80, $80, $80
    ; 137
    ; solid
    .byte   $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
    .align    256 * 8

    .export _SCREENMEM
_SCREENMEM:
    .res  1024
    .rodata
    .export _SCREENREG = (_SCREENMEM >> (2 + 4)) | (_CHARMEM >> 10) ; Use our char mem
    ;.export _SCREENREG = ($400 >> (2 + 4)) | (_CHARMEM >> 10) ; Use our char mem with stock screen ram position
    ;.export _SCREENREG = (_SCREENMEM >> (2 + 4)) | ($1000 >> 10) ; Use the ROM
    ;.export _SCREENREG = _CHARMEM >> 10 ; Use our char mem with stock screen ram position
