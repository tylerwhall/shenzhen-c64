#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <cbm.h>
#include <6502.h>

#include "screen.h"
#include "charset.h"

/* Cursor position */
static uint16_t posx;
static uint8_t posy;

#define CARD_WIDTH  4
#define CARD_HEIGHT 7
#define CARD_WIDTH_PX   (CARD_WIDTH * 8)
#define CARD_HEIGHT_PX  (CARD_HEIGHT * 8)

#define LOWER_STACKS_Y      (CARD_HEIGHT + 2)
#define NUM_LOWER_STACKS    8
#define UPPER_STACKS_Y      1

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
#define SPRITE_ID_MOUSE         0

#define SPRITE_CARD_MASK    ((1 << SPRITE_ID_CARD_BG) | \
                             (1 << SPRITE_ID_CARD_TOP) | \
                             (1 << SPRITE_ID_CARD_BOTTOM))
#define SPRITE_MOUSE_MASK   (1 << SPRITE_ID_MOUSE)

#define show_card_sprites() { VIC.spr_ena |= SPRITE_CARD_MASK; }
#define hide_card_sprites() { VIC.spr_ena &= ~SPRITE_CARD_MASK; }

/*
 * 3 sprites to draw the card.
 * 24x34 pixels
 * top: 24x21
 * bot: 24x13
 * bg:  24x17 (y doubled) 24x34
 */
#define SPRITE_CARD_WIDTH_PX    (24)
#define SPRITE_CARD_HEIGHT_PX   (34)

#define JOY_UP      (1 << 0)
#define JOY_DOWN    (1 << 1)
#define JOY_LEFT    (1 << 2)
#define JOY_RIGHT   (1 << 3)
#define JOY_BTN     (1 << 4)

#define JOY_SPEED 4

/* Debounced button state. */
static bool button_state;
static uint8_t button_state_frames;
#define button_changed()    (button_state_frames == 2) /* 2 frame debounce interval */

static void joy2_process(void)
{
    uint16_t card_posx;
    uint8_t card_posy;
    uint8_t joyval = ~CIA1.pra;
    bool cur_button_state;

    /* Handle button debounce */
    cur_button_state = !!(joyval & JOY_BTN);
    if (cur_button_state == button_state) {
        if (button_state_frames < 255) {
            button_state_frames++;
        }
    } else {
        button_state_frames = 1;
    }
    button_state = cur_button_state;

    if (button_changed()) {
        if (button_state) {
            show_card_sprites();
        } else {
            hide_card_sprites();
        }
    }

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
    card_posx = posx - (SPRITE_CARD_WIDTH_PX / 2);
    VIC.spr_pos[SPRITE_ID_CARD_BG].x = (uint8_t)card_posx;
    VIC.spr_pos[SPRITE_ID_CARD_TOP].x = (uint8_t)card_posx;
    VIC.spr_pos[SPRITE_ID_CARD_BOTTOM].x = (uint8_t)card_posx;
    VIC.spr_pos[SPRITE_ID_MOUSE].x = (uint8_t)posx;

    VIC.spr_hi_x &= ~(SPRITE_CARD_MASK | SPRITE_MOUSE_MASK);
    if (card_posx >> 8) {
        VIC.spr_hi_x |= SPRITE_CARD_MASK;
    }
    if (posx >> 8) {
        VIC.spr_hi_x |= SPRITE_MOUSE_MASK;
    }

    card_posy = posy - (SPRITE_CARD_HEIGHT_PX / 2);
    VIC.spr_pos[SPRITE_ID_CARD_BG].y = (uint8_t)card_posy;
    VIC.spr_pos[SPRITE_ID_CARD_TOP].y = (uint8_t)card_posy;
    VIC.spr_pos[SPRITE_ID_CARD_BOTTOM].y = (uint8_t)(card_posy + 21);
    VIC.spr_pos[SPRITE_ID_MOUSE].y = (uint8_t)posy;
}

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

static void sprite_setup(void)
{
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_BG] = (uint8_t)&SPRITE_PTR_CARD_BG;
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_TOP] = (uint8_t)&SPRITE_PTR_CARD_TOP;
    get_screen_mem()->sprite_ptr[SPRITE_ID_CARD_BOTTOM] = (uint8_t)&SPRITE_PTR_CARD_BOTTOM;
    get_screen_mem()->sprite_ptr[SPRITE_ID_MOUSE] = (uint8_t)&SPRITE_PTR_MOUSE;
    VIC.spr_exp_y = (1 << SPRITE_ID_CARD_BG);
    VIC.spr_color[SPRITE_ID_CARD_BG] = COLOR_WHITE;
    VIC.spr_color[SPRITE_ID_CARD_TOP] = COLOR_BLACK;
    VIC.spr_color[SPRITE_ID_CARD_BOTTOM] = COLOR_BLACK;
    VIC.spr_color[SPRITE_ID_MOUSE] = COLOR_BLACK;
    VIC.spr_ena = SPRITE_MOUSE_MASK; // Enable mouse
    joy2_process(); // Set up initial position

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
