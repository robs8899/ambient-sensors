#define OW_MAX_ATTEMPTS        10
#define OW_MAX_DEVICES          8
#define OW_ROM_SIZE             8
#define OW_PAD_SIZE             9

#define OW_SKIP_ROM          0xCC
#define OW_CONVERT_T         0x44
#define OW_MATCH_ROM         0x55
#define OW_SEARCH_ROM        0XF0
#define OW_READ_SCRATCHPAD   0xBE
#define OW_WRITE_SCRATCHPAD  0x4E
#define OW_COPY_SCRATCHPAD   0x48

struct ow_search_state {
  uint8_t last_discrepancy;
  uint8_t last_family_discrepancy;
  uint8_t is_last_device;
};

void ow_init(int port, int pin);
int ow_search_next(uint8_t *rom_code, struct ow_search_state *search_state);
int ow_search_all(uint8_t (*rom_codes)[OW_ROM_SIZE], int *num_devices);
int ow_send_convert(uint8_t *rom_code);
int ow_send_convert_all();
int ow_read_scratch_pad(uint8_t *rom_code, uint8_t *scratch_pad);

int ow_decode_temp(uint8_t *scratch_pad);
int ow_decode_alm1(uint8_t *scratch_pad);
int ow_decode_alm2(uint8_t *scratch_pad);
int ow_decode_rsln(uint8_t *scratch_pad);
