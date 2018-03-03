#include <string.h>
#include <stdint.h>

#include <cbm.h>
#include <6502.h>

#include "charset.h"
#include "screen.h"

#define BANK_REG (*(volatile char *)0x01)

/*
 * Copy the character ROM to our CHARMEM reservation
 */
void copy_character_rom(void)
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

static uint8_t old_screenreg;

void set_screen_addr(void)
{
    old_screenreg = VIC.addr;
    VIC.addr = (char)&SCREENREG;
}

void restore_screen_addr(void)
{
    VIC.addr = old_screenreg;
}

void init_screen(void)
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
    memset(addr, BG_CHAR, SCREEN_SIZE);
    memset(COLOR_RAM, BG_CHAR_COLOR, SCREEN_SIZE);
    /* Set Extended Background Color Mode */
    VIC.ctrl1 |= (1 << 6);
    VIC.bordercolor = COLOR_BLACK;
    VIC.bgcolor0 = COLOR_WHITE;
    VIC.bgcolor1 = COLOR_GREEN;
    VIC.bgcolor2 = COLOR_GRAY2;
    VIC.bgcolor3 = COLOR_GRAY2;
}

