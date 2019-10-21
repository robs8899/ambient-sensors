#define USE_SMALL_FONT 1
#define USE_BOLD_FONT  1
#define USE_BIG_FONT   1

#define SMALL_FONT_ID  1
#define BOLD_FONT_ID   2
#define BIG_FONT_ID    3

typedef struct FONT_TABLE {
  int width; // Character width in bits.
  int offset; // Offset in bytes into font bitmap.
} font_entry;

typedef struct FONT_INFO {
  uint8_t height;
  uint8_t spacing;
  uint8_t first;
  uint8_t last;
  const uint8_t *bitmap;
  const font_entry *table;
} font_info;

#if USE_SMALL_FONT
extern const uint8_t small_font_bitmap[];
extern const font_entry small_font_table[];
extern const font_info small_font_info;
#endif

#if USE_BOLD_FONT
extern const uint8_t bold_font_bitmap[];
extern const font_entry bold_font_table[];
extern const font_info bold_font_info;
#endif

#if USE_BIG_FONT
extern const uint8_t big_font_bitmap[];
extern const font_entry big_font_table[];
extern const font_info big_font_info;
#endif

#define DEFAULT_FONT &small_font_info
