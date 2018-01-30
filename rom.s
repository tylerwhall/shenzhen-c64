    .segment "CHARMEM"
    .export _CHARMEM
_CHARMEM:
    .res    33*8 ; 33 chars from the ROM table will be copied here. 33rd is space (blank) character
    ; Numbers 1-10
    .export _CARD_TOP_LEFT = (CARD_TOP_LEFT - _CHARMEM) / 8
CARD_TOP_LEFT:
    .incbin "images/6.bitmap"
    .incbin "images/6.bitmap"
    .incbin "images/6.bitmap"
    .incbin "images/6.bitmap"
    .incbin "images/6.bitmap"
    .incbin "images/6.bitmap"
    .incbin "images/7.bitmap"
    .incbin "images/7.bitmap"
    .incbin "images/7.bitmap"
    .incbin "images/7.bitmap"
    .export _CARD_BOTTOM_RIGHT = (CARD_BOTTOM_RIGHT - _CHARMEM) / 8
CARD_BOTTOM_RIGHT:
    .incbin "images/6.reversed.bitmap"
    .incbin "images/6.reversed.bitmap"
    .incbin "images/6.reversed.bitmap"
    .incbin "images/6.reversed.bitmap"
    .incbin "images/6.reversed.bitmap"
    .incbin "images/6.reversed.bitmap"
    .incbin "images/7.reversed.bitmap"
    .incbin "images/7.reversed.bitmap"
    .incbin "images/7.reversed.bitmap"
    .incbin "images/7.reversed.bitmap"
    .export _CARD_TOP = (CARD_TOP - _CHARMEM) / 8
CARD_TOP:
    .byte   $ff, $00, $00, $00, $00, $00, $00, $00
    .export _CARD_RIGHT = (CARD_RIGHT - _CHARMEM) / 8
CARD_RIGHT:
    .byte   $01, $01, $01, $01, $01, $01, $01, $01
    .export _CARD_BOTTOM = (CARD_BOTTOM - _CHARMEM) / 8
CARD_BOTTOM:
    .byte   $00, $00, $00, $00, $00, $00, $00, $ff
    .export _CARD_BOTTOM_LEFT = (CARD_BOTTOM_LEFT - _CHARMEM) / 8
CARD_BOTTOM_LEFT:
    .byte   $80, $80, $80, $80, $80, $80, $80, $7f
    .export _CARD_TOP_RIGHT = (CARD_TOP_RIGHT - _CHARMEM) / 8
CARD_TOP_RIGHT:
    .byte   $fe, $01, $01, $01, $01, $01, $01, $01
    .export _CARD_LEFT = (CARD_LEFT - _CHARMEM) / 8
CARD_LEFT:
    .byte   $80, $80, $80, $80, $80, $80, $80, $80
    ; solid
    .byte   $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
    .align  256 * 8

    .export _SCREENREG = (_SCREENMEM >> (2 + 4)) | (_CHARMEM >> 10) ; Use our char mem
    ;.export _SCREENREG = ($400 >> (2 + 4)) | (_CHARMEM >> 10) ; Use our char mem with stock screen ram position
    ;.export _SCREENREG = (_SCREENMEM >> (2 + 4)) | ($1000 >> 10) ; Use the ROM
    ;.export _SCREENREG = _CHARMEM >> 10 ; Use our char mem with stock screen ram position
    .export _SCREENMEM
_SCREENMEM:
    .res    1024
    .align  64
.export _SPRITE_PTR_CARD_TOP = SPRITE_CARD_TOP / 64
SPRITE_CARD_TOP:
    .incbin "images/cardsprite_top.bitmap"
    .align  64

.export _SPRITE_PTR_CARD_BOTTOM = SPRITE_CARD_BOTTOM / 64
SPRITE_CARD_BOTTOM:
    .incbin "images/cardsprite_bottom.bitmap"
    .align  64

.export _SPRITE_PTR_CARD_BG = SPRITE_CARD_BG / 64
SPRITE_CARD_BG:
    .incbin "images/cardsprite_bg.bitmap"
    .align  64
