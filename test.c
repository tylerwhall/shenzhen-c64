#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <6502.h>

#define BANK_REG (*(volatile char *)0x01)

struct screen_memory {
    char mem[1000];
    char pad[16];
    char sprite_ptr[8];
};

extern struct screen_memory SCREENMEM;
extern char CHARMEM[256 * 8];

#if 1
#define get_screen_mem() (&SCREENMEM)
#else
static struct screen_memory *get_screen_mem(void)
{
    return (struct screen_memory *)((VIC.addr & 0xf0) << (2 + 8 - 4));
}
#endif

extern char SCREENREG;
static uint8_t old_screenreg;

static void set_screen_addr(void)
{
    old_screenreg = VIC.addr;
    VIC.addr = (char)&SCREENREG;
}

static void restore_screen_addr(void)
{
    VIC.addr = old_screenreg;
}

/*
 * Since we're copying the 2nd 64 characters, this macro allows specifying the
 * character value in the upper 64 with its real value.
 */
#define char_offset(val) ((val) - 64)

/*
 * Copy the character ROM to our CHARMEM reservation
 */
static void copy_character_rom(void)
{
    char oldbank;
    SEI();
    // Unmap I/O to get to CHAR_ROM
    oldbank = BANK_REG;
    BANK_REG = oldbank & ~(1 << 2);
    /* Start from offset 64 of the 2nd char ROM table */
    memcpy(CHARMEM, (void *)(0xd800 + 64*8), 32*8);
    //memset(CHARMEM+1, 0, sizeof(CHARMEM)-1);
    // Restore banks
    BANK_REG = oldbank;
    CLI();
}

#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

void clear_screen(void)
{
    char *addr = &get_screen_mem()->mem[0];

#if 0
    /* Display all the characters */
    {
        int i;
        for (i = 0; i < SCREEN_SIZE; i++) {
            addr[i] = i;
        }
    }
#endif
    /* Checker pattern with BG color 2 */
    memset(addr, (2 << 6) | char_offset(94), SCREEN_SIZE / 2);
    memset(addr + SCREEN_SIZE / 2, (3 << 6) | char_offset(94), SCREEN_SIZE / 2);
    memset(COLOR_RAM, COLOR_GREEN, SCREEN_SIZE);
    /* Set Extended Background Color Mode */
    VIC.ctrl1 |= (1 << 6);
    VIC.bordercolor = COLOR_BLACK;
    VIC.bgcolor0 = COLOR_WHITE;
    VIC.bgcolor1 = COLOR_GREEN;
    VIC.bgcolor2 = COLOR_GRAY2;
    VIC.bgcolor3 = COLOR_GRAY2;
}

#define CARD_WIDTH  4
#define CARD_HEIGHT 7

#define LOWER_STACKS_Y      (CARD_HEIGHT + 2)
#define NUM_LOWER_STACKS    8
#define UPPER_STACKS_Y      1

/* ASM defines these symbols as character table offsets */
extern char CARD_TOP;
extern char CARD_TOP_LEFT;
extern char CARD_TOP_RIGHT;
extern char CARD_BOTTOM;
extern char CARD_BOTTOM_LEFT;
extern char CARD_BOTTOM_RIGHT;
extern char CARD_LEFT;
extern char CARD_RIGHT;
#define CARD(C) ((char)&CARD_ ##C)
#define CARD_TOP_LEFT(num) (CARD(TOP_LEFT) + num - 1)
#define CARD_BOTTOM_RIGHT(num) (CARD(BOTTOM_RIGHT) + num - 1)

static void card(uint8_t x, uint8_t y, uint8_t number, uint8_t color)
{
    char *char_addr = &get_screen_mem()->mem[0];
    register uint16_t offset = x + y * 40;
    int i;

    memset(&COLOR_RAM[offset], color, CARD_WIDTH);
    char_addr[offset++] = CARD_TOP_LEFT(number);
    char_addr[offset++] = CARD(TOP);
    char_addr[offset++] = CARD(TOP);
    char_addr[offset++] = CARD(TOP_RIGHT);
    offset += SCREEN_WIDTH - CARD_WIDTH;

    for (i = 0; i < CARD_HEIGHT - 2; i++) {
        memset(&COLOR_RAM[offset], color, CARD_WIDTH);
        char_addr[offset++] = CARD(LEFT);
        char_addr[offset++] = ' ';
        char_addr[offset++] = ' ';
        char_addr[offset++] = CARD(RIGHT);
        offset += SCREEN_WIDTH - CARD_WIDTH;
    }

    memset(&COLOR_RAM[offset], color, CARD_WIDTH);
    char_addr[offset++] = CARD(BOTTOM_LEFT);
    char_addr[offset++] = CARD(BOTTOM);
    char_addr[offset++] = CARD(BOTTOM);
    char_addr[offset++] = CARD_BOTTOM_RIGHT(number);
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

extern char SPRITE_PTR_CARD_TOP;
extern char SPRITE_PTR_CARD_BOTTOM;
extern char SPRITE_PTR_CARD_BG;

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
}


int main(void)
{
    printf("Hello world port: 0x%x\n", *(unsigned char *)(0x01));
    printf("Screen at 0x%x\n", (uint16_t)get_screen_mem());
#if 1
    sprite_setup();
    copy_character_rom();
    set_screen_addr();
    clear_screen();
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
