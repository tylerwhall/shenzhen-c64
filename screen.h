#ifndef _SCREEN_H_
#define _SCREEN_H_

void copy_character_rom(void);
void set_screen_addr(void);
void restore_screen_addr(void);
void init_screen(void);

#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

/*
 * Since we're copying the 2nd 64 characters, this macro allows specifying the
 * character value in the upper 64 with its real value.
 */
#define char_offset(val) ((val) - 64)

#define BG_CHAR_COLOR   COLOR_GREEN
#define BG_CHAR         ((2 << 6) | char_offset(94)) /* Checker pattern with BG color 2 */
#endif
