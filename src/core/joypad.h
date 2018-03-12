#ifndef joypad_h
#define joypad_h

typedef struct __attribute__((packed)) {
    struct {
        union {
            uint8_t raw;
            struct {
                uint8_t buttons: 4;
                uint8_t directions: 4;
            };
            struct {
                uint8_t buttonA: 1;
                uint8_t buttonB: 1;
                uint8_t select: 1;
                uint8_t start: 1;
                uint8_t right: 1;
                uint8_t left: 1;
                uint8_t up: 1;
                uint8_t down: 1;
            };
        };
    } keys;
    union {
        uint8_t raw;
        struct {
            uint8_t keys: 4;
            uint8_t select_direction_keys: 1;
            uint8_t select_button_keys: 1;
        };
    } JOYP;
} Joypad;

void joypad_init(void);
bool joypad_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool joypad_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void joypad_check(struct Context *this);

#endif /* joypad_h */
