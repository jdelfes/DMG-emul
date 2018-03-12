#ifndef timer_h
#define timer_h

typedef struct __attribute__((packed)) {
    struct {
        uint64_t last_update;
        union {
            struct {
                uint8_t _;
                uint8_t value;
            };
            uint16_t internal;
        };
    } DIV;
    uint8_t TIMA;
    union {
        uint8_t raw;
        struct {
            uint8_t clock_select: 2;
            uint8_t enable: 1;
        };
    } TAC;
    uint8_t TMA;
} Timer;

bool timer_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool timer_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void timer_check(struct Context *this);

#endif /* timer_h */
