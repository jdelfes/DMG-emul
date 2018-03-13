#ifndef mbc5_h
#define mbc5_h

typedef struct __attribute__((packed)) {
    uint8_t *rom_extra_bank;
    union {
        struct {
            uint8_t lower_bits;
            uint8_t higher_bit: 1;
        };
        uint16_t raw;
    } rom_bank_number;
    bool eram_enabled;
    uint8_t *eram_data; // A000-BFFF    8KB External RAM
    uint8_t *eram_ptr;
} MBC5Data;

void mbc5_init(struct Context *this);
bool mbc5_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool mbc5_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* mbc5_h */
