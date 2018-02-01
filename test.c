#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <6502.h>

#include "screen.h"
#include "charset.h"

#define CARD_WIDTH  4
#define CARD_HEIGHT 7

#define LOWER_STACKS_Y      (CARD_HEIGHT + 2)
#define NUM_LOWER_STACKS    8
#define UPPER_STACKS_Y      1

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

static void card(uint8_t x, uint8_t y, uint8_t number, uint8_t color)
{
    char *char_addr = &get_screen_mem()->mem[0];
    register uint16_t offset = x + y * 40;
    int i;

    memset(&COLOR_RAM[offset], color, CARD_WIDTH);
    char_addr[offset++] = CARD_IDX_TOP_LEFT(number);
    char_addr[offset++] = CARD_IDX(TOP);
    char_addr[offset++] = CARD_IDX(TOP);
    char_addr[offset++] = CARD_IDX(TOP_RIGHT);
    offset += SCREEN_WIDTH - CARD_WIDTH;

    for (i = 0; i < CARD_HEIGHT - 2; i++) {
        memset(&COLOR_RAM[offset], color, CARD_WIDTH);
        char_addr[offset++] = CARD_IDX(LEFT);
        char_addr[offset++] = ' ';
        char_addr[offset++] = ' ';
        char_addr[offset++] = CARD_IDX(RIGHT);
        offset += SCREEN_WIDTH - CARD_WIDTH;
    }

    memset(&COLOR_RAM[offset], color, CARD_WIDTH);
    char_addr[offset++] = CARD_IDX(BOTTOM_LEFT);
    char_addr[offset++] = CARD_IDX(BOTTOM);
    char_addr[offset++] = CARD_IDX(BOTTOM);
    char_addr[offset++] = CARD_IDX_BOTTOM_RIGHT(number);
}

static void cards(void)
{
    int i;

#define STEP (CARD_WIDTH + 1)
    card(STEP * 5, UPPER_STACKS_Y, 7, COLOR_RED);
    card(STEP * 6, UPPER_STACKS_Y, 7, COLOR_GREEN);
    card(STEP * 7, UPPER_STACKS_Y, 7, COLOR_BLACK);

    for (i = 0; i < (NUM_LOWER_STACKS * STEP); i += STEP) {
        card(i, LOWER_STACKS_Y, 7, COLOR_RED);
    }
    for (i = 0; i < (NUM_LOWER_STACKS * STEP / 2); i += STEP) {
        card(i, LOWER_STACKS_Y + 1, 6, COLOR_GREEN);
    }
    for (i = 0; i < (NUM_LOWER_STACKS * STEP / 4); i += STEP) {
        card(i, LOWER_STACKS_Y + 2, 6, COLOR_BLACK);
    }
#undef STEP
}

#define RASTER_MIN      51
#define RASTER_MAX      (RASTER_MIN + SCREEN_HEIGHT * 8)

#define SPRITE_XOFFSET  24
#define SPRITE_YOFFSET  (29 + 21)

#define SPRITE_XMIN (SPRITE_XOFFSET)
#define SPRITE_XMAX (SPRITE_XOFFSET + (SCREEN_WIDTH * 8) - 2) /* Leave a couple extra pixels so it's still visible */
#define SPRITE_YMIN (SPRITE_YOFFSET)
#define SPRITE_YMAX (SPRITE_YOFFSET + (SCREEN_HEIGHT * 8) - 2) /* Leave a couple extra pixels so it's still visible */

#define SPRITE_ID_CARD_BG       7
#define SPRITE_ID_CARD_TOP      6
#define SPRITE_ID_CARD_BOTTOM   5

#define SPRITE_CARD_MASK    ((1 << SPRITE_ID_CARD_BG) | \
                             (1 << SPRITE_ID_CARD_TOP) | \
                             (1 << SPRITE_ID_CARD_BOTTOM))

#define JOY_UP      (1 << 0)
#define JOY_DOWN    (1 << 1)
#define JOY_LEFT    (1 << 2)
#define JOY_RIGHT   (1 << 3)

#define JOY_SPEED 4

static uint16_t posx;
static uint8_t posy;

static void joy2_process(void)
{
    uint8_t joyval = ~CIA1.pra;

    if (joyval & JOY_UP) {
        posy -= JOY_SPEED;
    }
    if (joyval & JOY_DOWN) {
        posy += JOY_SPEED;
    }
    if (posy > SPRITE_YMAX) {
        posy = SPRITE_YMAX;
    }
    if (posy < SPRITE_YMIN) {
        posy = SPRITE_YMIN;
    }
    if (joyval & JOY_LEFT) {
        posx -= JOY_SPEED;
    }
    if (joyval & JOY_RIGHT) {
        posx += JOY_SPEED;
    }
    if (posx > SPRITE_XMAX) {
        posx = SPRITE_XMAX;
    }
    if (posx < SPRITE_XMIN) {
        posx = SPRITE_XMIN;
    }
    VIC.spr_pos[SPRITE_ID_CARD_BG].x = (uint8_t)posx;
    VIC.spr_pos[SPRITE_ID_CARD_TOP].x = (uint8_t)posx;
    VIC.spr_pos[SPRITE_ID_CARD_BOTTOM].x = (uint8_t)posx;

    VIC.spr_hi_x &= ~SPRITE_CARD_MASK;
    if (posx >> 8) {
        VIC.spr_hi_x |= SPRITE_CARD_MASK;
    }

    VIC.spr_pos[SPRITE_ID_CARD_BG].y = (uint8_t)posy;
    VIC.spr_pos[SPRITE_ID_CARD_TOP].y = (uint8_t)posy;
    VIC.spr_pos[SPRITE_ID_CARD_BOTTOM].y = (uint8_t)(posy + 21);
}

extern uint8_t SPRITE_PTR_CARD_TOP;
extern uint8_t SPRITE_PTR_CARD_BOTTOM;
extern uint8_t SPRITE_PTR_CARD_BG;
extern uint8_t SPRITE_CARD_TOP[63];
extern uint8_t SPRITE_CARD_BOTTOM[63];

/*
 * Copy a character (8x8) bitmap to an 8x8 location in a sprite of width 24.
 * Must be byte aligned.
 */
static void copy_char_to_sprite(uint8_t *c, uint8_t *sprite)
{
    int i;

    for (i = 0; i < 8; i++) {
        *sprite = c[i];
        sprite += 3;
    }
}

static void sprite_card_personify(uint8_t number)
{

    copy_char_to_sprite(CARD_TOP_LEFT + number * 8, SPRITE_CARD_TOP);
    /* Bottom right is row 26, col 2 */
    copy_char_to_sprite(CARD_BOTTOM_RIGHT + number * 8, SPRITE_CARD_BOTTOM + (26 - 21) * 3 + 2);
}

/*
 * 3 sprites to draw the card.
 * 24x34 pixels
 * top: 24x21
 * bot: 24x13
 * bg:  24x17 (y doubled) 24x34
 */
static void sprite_setup(void)
{
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_BG] = (uint8_t)&SPRITE_PTR_CARD_BG;
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_TOP] = (uint8_t)&SPRITE_PTR_CARD_TOP;
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_BOTTOM] = (uint8_t)&SPRITE_PTR_CARD_BOTTOM;
    VIC.spr_exp_y = (1 << SPRITE_ID_CARD_BG);
    VIC.spr_color[SPRITE_ID_CARD_BG] = COLOR_WHITE;
    VIC.spr_color[SPRITE_ID_CARD_TOP] = COLOR_BLACK;
    VIC.spr_color[SPRITE_ID_CARD_BOTTOM] = COLOR_BLACK;
    joy2_process(); // Set up initial position
    VIC.spr_ena = SPRITE_CARD_MASK; // Enable sprites

    sprite_card_personify(7);
}


int main(void)
{
    printf("Hello world port: 0x%x\n", *(unsigned char *)(0x01));
    printf("Screen at 0x%x\n", (uint16_t)get_screen_mem());
#if 1
    sprite_setup();
    copy_character_rom();
    set_screen_addr();
    init_screen();
    cards();
#endif
    //printf("Screenreg 0x %x\n", (char)&SCREENREG);
    //printf("Press return to exit");
    while (1) {
        while (VIC.rasterline < RASTER_MAX);

        joy2_process();

        while (VIC.rasterline > RASTER_MAX);
    }
    while (cbm_k_getin() != '1');

    restore_screen_addr();

    return 0;
}
