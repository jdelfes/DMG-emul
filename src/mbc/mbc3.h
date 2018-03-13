#ifndef mbc3_h
#define mbc3_h

typedef struct __attribute__((packed)) {
    uint8_t *rom_extra_bank;
    uint8_t rom_bank_number;
    bool eram_enabled;
    uint8_t *eram_data; // A000-BFFF    8KB External RAM
    uint8_t *eram_ptr;
    bool rtc_mode;
    uint8_t rtc_register;
} MBC3Data;

void mbc3_init(struct Context *this);
bool mbc3_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool mbc3_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* mbc3_h */
