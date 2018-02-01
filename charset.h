#ifndef _CHARSET_H_
#define _CHARSET_H_

#define SCREENMEM_SIZE 1000
struct screen_memory {
    char mem[SCREENMEM_SIZE];
    char pad[16];
    char sprite_ptr[8];
};

extern struct screen_memory SCREENMEM;
/* Get a pointer to screen ram */
#define get_screen_mem() (&SCREENMEM)
/* The offset value that VIC.addr should be set to */
extern char SCREENREG;

extern char CHARMEM[256 * 8];

extern char CARD_TOP_LEFT[8]; /* Acutal address */
extern char CARD_BOTTOM_RIGHT[8]; /* Acutal address */

/* ASM defines these symbols as character table offsets */
extern char CARD_IDX_TOP;
extern char CARD_IDX_TOP_LEFT;
extern char CARD_IDX_TOP_RIGHT;
extern char CARD_IDX_BOTTOM;
extern char CARD_IDX_BOTTOM_LEFT;
extern char CARD_IDX_BOTTOM_RIGHT;
extern char CARD_IDX_LEFT;
extern char CARD_IDX_RIGHT;
#define CARD_IDX(C) ((char)&CARD_IDX_ ##C)
#define CARD_IDX_TOP_LEFT(num) (CARD_IDX(TOP_LEFT) + num - 1)
#define CARD_IDX_BOTTOM_RIGHT(num) (CARD_IDX(BOTTOM_RIGHT) + num - 1)

extern uint8_t SPRITE_PTR_CARD_TOP;
extern uint8_t SPRITE_PTR_CARD_BOTTOM;
extern uint8_t SPRITE_PTR_CARD_BG;
extern uint8_t SPRITE_PTR_MOUSE;
extern uint8_t SPRITE_CARD_TOP[63];
extern uint8_t SPRITE_CARD_BOTTOM[63];

#endif
