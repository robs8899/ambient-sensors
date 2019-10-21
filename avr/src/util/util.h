
/** creates a hex dump from byte buffer */
char *hex_dmp(char *buf, uint8_t *bytes, int size);

/** creates a hex string from byte buffer */
char *hex_str(char *buf, uint8_t *bytes, int size);

/** creates a bit pattern from byte */
char *patt(char *buf, uint8_t byte);

/** converts an unsigned decimal integer to string */
char *dtoa(char *buf, uint16_t val, int dig);

/** converts a scaled decimal integer to string */
char *stoa(char *buf, int32_t val, uint8_t prc, uint8_t scl);

/** formats a string with '$' placeholders */
char *fmt(char *buf, const char *fmt, ...);
