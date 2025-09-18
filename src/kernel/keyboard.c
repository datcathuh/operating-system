#include "keyboard.h"
#include "io.h"
#include <stdbool.h>

static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*',0,' ',
};

static const char scancode_to_ascii_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?', 0,'*',0,' ',
};

static bool shift_pressed = false;

char keyboard_get_key(void) {
    while ((inb(0x64) & 1) == 0); // wait until data ready
    unsigned char sc = inb(0x60);

    // Handle key release
    if (sc & 0x80) {
        sc &= 0x7F; // remove release bit

        if (sc == 42 || sc == 54) shift_pressed = false;

        return 0;
    } else {
        // track shift press
        if (sc == 42 || sc == 54) {
            shift_pressed = true;
            return 0;
        }

        if (sc < 128) {
            return shift_pressed ? scancode_to_ascii_shift[sc] : scancode_to_ascii[sc];
        }

        return 0;
    }
}
