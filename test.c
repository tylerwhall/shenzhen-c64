#include <stdio.h>
#include <stdint.h>
#include <cbm.h>

#define inb(addr) (*(char *)addr)

static char *get_screen_addr(void)
{
    return (char *)((VIC.addr & 0xf0) << (2 + 8 - 4));
}

#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25

void clear_screen(void)
{
    int i;
    char *addr = get_screen_addr();

    for (i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++) {
        addr[i] = ' ';
    }
    VIC.bordercolor = 0x5;
    VIC.bgcolor0 = 0xd;
}

#define CARD_WIDTH  4
#define CARD_HEIGHT 6

#define LOWER_STACKS_Y      (CARD_HEIGHT + 1)
#define NUM_LOWER_STACKS    8

static void card(uint8_t x, uint8_t y, uint8_t color)
{
    int i;
    char *char_addr = get_screen_addr();
    uint16_t offset = x + y * 40;

    for (i = 0; i < CARD_HEIGHT; i++) {
        COLOR_RAM[offset] = color;
        char_addr[offset++] = '7';
        COLOR_RAM[offset] = color;
        char_addr[offset++] = '-';
        COLOR_RAM[offset] = color;
        char_addr[offset++] = '-';
        COLOR_RAM[offset] = color;
        char_addr[offset++] = '|';
        offset += 40 - CARD_WIDTH;
    }
}

static void cards(void)
{
    int i;

#define STEP (CARD_WIDTH + 1)
    for (i = 0; i < (NUM_LOWER_STACKS * STEP); i += STEP) {
        card(i, LOWER_STACKS_Y, COLOR_RED);
    }
#undef STEP
}

int main(void)
{
    printf("Hello world port: 0x%x\n", *(unsigned char *)(0x01));
    printf("Screen at 0x%x\n", get_screen_addr());
    clear_screen();
    cards();
    printf("Press return to exit");
    while (cbm_k_getin() != '\n');

    return 0;
}
