#include "isr/irq_keyboard.h"
#include "keyboard.h"
#include "types.h"

static bool shift_pressed = false;

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

char keyboard_get_key(void) {
	uint8_t sc;
	bool key_fetched = false;
	while(!key_fetched) {
		__asm__ volatile("hlt");
		key_fetched = irq_keyboard_consume_key(&sc);
		if(key_fetched) {
			// Handle key release
			if (sc & 0x80) {
				sc &= 0x7F;

				if (sc == 42 || sc == 54) {
					shift_pressed = false;
				}

				return 0;
			} else {
				// looks for shift presses
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
	}
	return 0;
}
