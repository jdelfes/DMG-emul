#ifndef mbc2_h
#define mbc2_h

typedef struct __attribute__((packed)) {
    uint8_t *rom_extra_bank;
    uint8_t rom_bank_number;
    bool ram_enabled;
    struct Byte ram_data[512]; // A000-A1FF    512x4bits built-in
} MBC2Data;

void mbc2_init(struct Context *this);
bool mbc2_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool mbc2_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* mbc2_h */

