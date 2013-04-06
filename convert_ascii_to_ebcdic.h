/* Takes an ASCII character and returns the EBCDIC equivalent. */
char ascii_to_ebcdic_c (char ascii);

/* Takes a null-terminated ASCII string and a pointer to a buffer large enough
 * to hold the EBCDIC equivalent, and fills the EBCIDC buffer with the correct
 * transcoding. The buffers may be the same, but if they overlap and don't
 * have the same start, strange things may happen.
 */
char* ascii_to_ebcdic (char* ascii, char* ebcdic);