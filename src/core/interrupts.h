#ifndef interrupts_h
#define interrupts_h

typedef struct __attribute__((packed)) {
    bool IME;
    bool interrupt_happened;
    union {
        uint8_t raw;
        struct {
            uint8_t v_blank: 1;
            uint8_t lcd_stat: 1;
            uint8_t timer: 1;
            uint8_t serial: 1;
            uint8_t joypad: 1;
        };
    } IF;
    union {
        uint8_t raw;
        struct {
            uint8_t v_blank: 1;
            uint8_t lcd_stat: 1;
            uint8_t timer: 1;
            uint8_t serial: 1;
            uint8_t joypad: 1;
        };
    } IE;
} Interrupts;

bool interrupts_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool interrupts_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void interrupts_check(struct Context *this);

#endif /* interrupts_h */
