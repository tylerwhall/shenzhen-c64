#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <6502.h>

#define BANK_REG (*(volatile char *)0x01)

extern char SCREENMEM[1000];
extern char CHARMEM[256 * 8];

#if 0
#define get_screen_addr() ((char *)SCREENMEM)
#else
static char *get_screen_addr(void)
{
    return (char *)((VIC.addr & 0xf0) << (2 + 8 - 4));
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
 * Copy the character ROM to our CHARMEM reservation
 */
static void copy_character_rom(void)
{
    char oldbank;
    SEI();
    // Unmap I/O to get to CHAR_ROM
    oldbank = BANK_REG;
    BANK_REG = oldbank & ~(1 << 2);
    memcpy(CHARMEM, (void *)0xd800, sizeof(CHARMEM)/2);
    //memset(CHARMEM+1, 0, sizeof(CHARMEM)-1);
    // Restore banks
    BANK_REG = oldbank;
    CLI();
}

#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25

void clear_screen(void)
{
    char *addr = get_screen_addr();

    memset(addr, 137, SCREEN_WIDTH * SCREEN_HEIGHT);
    memset(COLOR_RAM, COLOR_LIGHTGREEN, 1000);
    VIC.bordercolor = COLOR_LIGHTGREEN;
    VIC.bgcolor0 = COLOR_WHITE;
}

#define CARD_WIDTH  4
#define CARD_HEIGHT 6

#define LOWER_STACKS_Y      (CARD_HEIGHT + 2)
#define NUM_LOWER_STACKS    8
#define UPPER_STACKS_Y      1

static void card(uint8_t x, uint8_t y, uint8_t number, uint8_t color)
{
    char *char_addr = get_screen_addr();
    uint16_t offset = x + y * 40;

    memset(&COLOR_RAM[offset], color, 4);
    if (number == 7)
        char_addr[offset++] = 129;
    else
        char_addr[offset++] = 128;
    char_addr[offset++] = 130;
    char_addr[offset++] = 130;
    char_addr[offset++] = 135;
    offset += SCREEN_WIDTH - CARD_WIDTH;

    memset(&COLOR_RAM[offset], color, 4);
    char_addr[offset++] = 136;
    char_addr[offset++] = ' ';
    char_addr[offset++] = ' ';
    char_addr[offset++] = 131;
    offset += SCREEN_WIDTH - CARD_WIDTH;

    memset(&COLOR_RAM[offset], color, 4);
    char_addr[offset++] = 136;
    char_addr[offset++] = ' ';
    char_addr[offset++] = ' ';
    char_addr[offset++] = 131;
    offset += SCREEN_WIDTH - CARD_WIDTH;

    memset(&COLOR_RAM[offset], color, 4);
    char_addr[offset++] = 136;
    char_addr[offset++] = ' ';
    char_addr[offset++] = ' ';
    char_addr[offset++] = 131;
    offset += SCREEN_WIDTH - CARD_WIDTH;

    memset(&COLOR_RAM[offset], color, 4);
    char_addr[offset++] = 136;
    char_addr[offset++] = ' ';
    char_addr[offset++] = ' ';
    char_addr[offset++] = 131;
    offset += SCREEN_WIDTH - CARD_WIDTH;

    memset(&COLOR_RAM[offset], color, 4);
    char_addr[offset++] = 134;
    char_addr[offset++] = 133;
    char_addr[offset++] = 133;
    char_addr[offset++] = 132;
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

int main(void)
{
    printf("Hello world port: 0x%x\n", *(unsigned char *)(0x01));
    printf("Screen at 0x%x\n", get_screen_addr());
    copy_character_rom();
    set_screen_addr();
    clear_screen();
    cards();
    //printf("Screenreg 0x %x\n", (char)&SCREENREG);
    //printf("Press return to exit");
    while (cbm_k_getin() != '1');

    restore_screen_addr();

    return 0;
}
