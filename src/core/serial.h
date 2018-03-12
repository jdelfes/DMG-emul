#ifndef serial_h
#define serial_h

typedef struct __attribute__((packed)) {
    uint8_t SB;
    union {
        uint8_t raw;
        struct {
            uint8_t shift_clock: 1;
            uint8_t clock_speed: 1;
            uint8_t : 5;
            uint8_t transfer_start: 1;
        };
    } SC;
    uint64_t last_transfer_started;
} Serial;

void serial_tick(struct Context *this);
bool serial_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool serial_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* serial_h */
