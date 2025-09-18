// Minimal freestanding kernel example with VGA text output + keyboard (polling).
// Compile with a freestanding toolchain; link as kernel binary (no libc).

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

volatile u8 *vidptr = (volatile u8*)0xb8000;
const u32 VGA_WIDTH = 80;
const u32 VGA_HEIGHT = 25;
const u32 VGA_MEM_SIZE = VGA_WIDTH * VGA_HEIGHT * 2;

// ==== I/O helpers ====
static inline u8 inb(u16 port) {
    u8 value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}

// ==== VGA helpers ====
static void clear_screen(void) {
    for (u32 j = 0; j < VGA_MEM_SIZE; j += 2) {
        vidptr[j] = ' ';
        vidptr[j + 1] = 0x07;
    }
}

static void scroll(void) {
    // Move everything up one line
    for (u32 i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; ++i) {
        vidptr[i] = vidptr[i + VGA_WIDTH * 2];
    }
    // Clear last line
    for (u32 i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_MEM_SIZE; i += 2) {
        vidptr[i] = ' ';
        vidptr[i + 1] = 0x07;
    }
}

static void put_char_at(u32 *byte_index, char c, u8 attr) {
    if (c == '\n') {
        u32 row = (*byte_index / 2) / VGA_WIDTH;
        row++;
        if (row >= VGA_HEIGHT) {
            scroll();
            row = VGA_HEIGHT - 1;
        }
        *byte_index = row * VGA_WIDTH * 2;
        return;
    }
    if (c == '\b') {
        if (*byte_index >= 2) {
            *byte_index -= 2;
            vidptr[*byte_index] = ' ';
            vidptr[*byte_index + 1] = attr;
        }
        return;
    }

    vidptr[*byte_index] = c;
    vidptr[*byte_index + 1] = attr;
    *byte_index += 2;

    if ((*byte_index / 2) % VGA_WIDTH == 0) {
        put_char_at(byte_index, '\n', attr);
    }
}

static void put_string(u32 *pos, const char *s) {
    while (*s) {
        put_char_at(pos, *s++, 0x07);
    }
}

// ==== Keyboard maps ====
static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*',0,' ',
};

static const char scancode_to_ascii_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
};

// ==== Keyboard input ====
static char get_key(int *shift_state) {
    // Wait until status register (0x64) bit 0 = 1 (output buffer full)
    while ((inb(0x64) & 1) == 0);

    u8 sc = inb(0x60);

    if (sc & 0x80) {
        // key release
        u8 released = sc & 0x7F;
        if (released == 0x2A || released == 0x36) {
            *shift_state = 0;
        }
        return 0;
    } else {
        if (sc == 0x2A || sc == 0x36) { // shift
            *shift_state = 1;
            return 0;
        }
        if (sc == 0x0E) return '\b'; // backspace
        if (sc == 0x1C) return '\n'; // enter

        if (*shift_state) {
            return sc < 128 ? scancode_to_ascii_shift[sc] : 0;
        } else {
            return sc < 128 ? scancode_to_ascii[sc] : 0;
        }
    }
}

// ==== Kernel entry point ====
void kmain(void) {
    clear_screen();

    u32 pos = 0;
    put_string(&pos, "Hello from someos ig idk!\n");
    put_string(&pos, "t.me/x3ghx\n");
    put_string(&pos, "> ");

    int shift = 0;
    while (1) {
        char c = get_key(&shift);
        if (c) {
            put_char_at(&pos, c, 0x07);
        }
    }
}
