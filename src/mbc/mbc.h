#ifndef mbc_h
#define mbc_h

typedef enum {
    ROM_ONLY = 0,
    MBC1 = 1,
    MBC2 = 2,
    MBC3 = 3,
//    MBC4 = 4,
    MBC5 = 5,
//    MBC6 = 6,
//    MBC7 = 7,
} MBCType;

typedef struct __attribute__((packed)) {
    MBCType type;
    void *data;
} MBC;

void mbc_init(struct Context *this);
bool mbc_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool mbc_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* mbc_h */
