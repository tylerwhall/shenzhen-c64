    .importzp ptr1
    .import _card_draw_colorpos
    .export _asm_set_card_row_color
_asm_set_card_row_color:
    ldx _card_draw_colorpos
    stx ptr1
    ldx _card_draw_colorpos+1
    stx ptr1+1

    ldy #0
    sta (ptr1),y
    iny
    sta (ptr1),y
    iny
    sta (ptr1),y
    iny
    sta (ptr1),y
    rts

    .import _card_draw_screenpos, _CARD_IDX_TOP_LEFT, _CARD_IDX_TOP, _CARD_IDX_TOP_RIGHT
    .export _asm_draw_card_top
_asm_draw_card_top:
    ldx _card_draw_screenpos
    stx ptr1
    ldx _card_draw_screenpos+1
    stx ptr1+1

    and #$0f
    clc
    adc #<(_CARD_IDX_TOP_LEFT) - 1 ; Card showing 1 is at index 0, so subtract 1

    ldy #0
    sta (ptr1),y
    iny
    lda #<(_CARD_IDX_TOP)
    sta (ptr1),y
    iny
    sta (ptr1),y
    iny
    lda #<(_CARD_IDX_TOP_RIGHT)
    sta (ptr1),y
    rts

    .import _CARD_IDX_BOTTOM_LEFT, _CARD_IDX_BOTTOM, _CARD_IDX_BOTTOM_RIGHT
    .export _asm_draw_card_bottom
_asm_draw_card_bottom:
    ldx _card_draw_screenpos
    stx ptr1
    ldx _card_draw_screenpos+1
    stx ptr1+1

    and #$0f
    clc
    adc #<(_CARD_IDX_BOTTOM_RIGHT) - 1; Card showing 1 is at index 0, so subtract 1
    tax

    ldy #0
    lda #<(_CARD_IDX_BOTTOM_LEFT)
    sta (ptr1),y
    iny
    lda #<(_CARD_IDX_BOTTOM)
    sta (ptr1),y
    iny
    sta (ptr1),y
    iny
    txa
    sta (ptr1),y
    rts

    .import _CARD_IDX_LEFT, _CARD_IDX_RIGHT
    .export _asm_draw_card_middle
_asm_draw_card_middle:
    ldx _card_draw_screenpos
    stx ptr1
    ldx _card_draw_screenpos+1
    stx ptr1+1

    ldy #0
    lda #<(_CARD_IDX_LEFT)
    sta (ptr1),y
    iny
    lda #' '
    sta (ptr1),y
    iny
    sta (ptr1),y
    iny
    lda #<(_CARD_IDX_RIGHT)
    sta (ptr1),y
    rts
