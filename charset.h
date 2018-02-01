#ifndef _CHARSET_H_
#define _CHARSET_H_

struct screen_memory {
    char mem[1000];
    char pad[16];
    char sprite_ptr[8];
};

extern struct screen_memory SCREENMEM;
/* Get a pointer to screen ram */
#define get_screen_mem() (&SCREENMEM)
/* The offset value that VIC.addr should be set to */
extern char SCREENREG;

extern char CHARMEM[256 * 8];

#endif
