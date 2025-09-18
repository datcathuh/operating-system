void kmain(void) {
    const char *str1 = "Hello from someos ig idk!";
    const char *str2 = "t.me/x3ghx";
    char *vidptr = (char*)0xb8000;  // Video memory starts here
    
    unsigned int i = 0;
    unsigned int j = 0;

    // Clear screen (25 lines x 80 columns)
    while (j < 80 * 25 * 2) {
        vidptr[j] = ' ';
        vidptr[j+1] = 0x07;  // Light grey on black
        j = j + 2;
    }

    j = 0;

    // Write first string to screen
    while (str1[j] != '\0') {
        vidptr[i] = str1[j];
        vidptr[i+1] = 0x07;
        ++j;
        i = i + 2;
    }

    // Move to next line (80 columns → 160 bytes)
    i = 160;

    j = 0;
    // Write second string to screen
    while (str2[j] != '\0') {
        vidptr[i] = str2[j];
        vidptr[i+1] = 0x07;
        ++j;
        i = i + 2;
    }

    return;
}
