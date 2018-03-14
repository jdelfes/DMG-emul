#ifndef opcodes_h
#define opcodes_h

struct Opcode {
    uint8_t code;
    uint8_t size;
    union {
        uint8_t u8;
        int8_t i8;
    };
    uint16_t u16;
};

struct Opcode next_opcode(struct Context *this);

#endif /* opcodes_h */
