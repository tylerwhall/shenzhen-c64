#ifndef _SCREEN_H_
#define _SCREEN_H_

void copy_character_rom(void);
void set_screen_addr(void);
void restore_screen_addr(void);
void init_screen(void);

#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

#endif
