#ifndef mbc_h
#define mbc_h

enum MMCType {
    RomOnly = 0,
    MBC1 = 1
};

struct MBC {
    int mbc_type;
    uint8_t *rom_extra_bank;
    union {
        struct {
            uint8_t lower_bits: 5;
            uint8_t higher_bits: 2;
        };
        uint8_t raw;
    } rom_bank_number;
    bool eram_enabled;
    uint8_t *eram_data; // A000-BFFF    8KB External RAM
    uint8_t *eram_ptr;
    uint8_t rom_ram_mode_select;
};

void mbc_init(struct Context *this);
bool mbc_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool mbc_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* mbc_h */
