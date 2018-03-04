#ifndef cpu_h
#define cpu_h

struct Context;
struct Opcode;

struct __attribute__((packed)) Byte {
    union {
        uint8_t byte;
        struct {
            uint8_t n0: 4;
            uint8_t n1: 4;
        };
        struct {
            uint8_t b0: 1;
            uint8_t b1: 1;
            uint8_t b2: 1;
            uint8_t b3: 1;
            uint8_t b4: 1;
            uint8_t b5: 1;
            uint8_t b6: 1;
            uint8_t b7: 1;
        };
    };
};

struct __attribute__((packed)) CPU {
    struct {
        union {
            uint16_t AF;
            struct {
                union {
                    uint8_t F;
                    struct {
                        uint8_t : 4;
                        uint8_t cy: 1;
                        uint8_t h: 1;
                        uint8_t n: 1;
                        uint8_t zf: 1;
                    } flags;
                };
                struct Byte A;
            };
        };
        union {
            uint16_t BC;
            struct {
                struct Byte C;
                struct Byte B;
            };
        };
        union {
            uint16_t DE;
            struct {
                struct Byte E;
                struct Byte D;
            };
        };
        union {
            uint16_t HL;
            struct {
                struct Byte L;
                struct Byte H;
            };
        };
        uint16_t SP;
        uint16_t PC;
    } registers;
};

uint8_t cpu_execute(struct Context *this, const struct Opcode opcode);
void push16(struct Context *this, uint16_t value);
uint16_t pop16(struct Context *this);

#endif /* cpu_h */
