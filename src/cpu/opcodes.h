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

extern uint8_t OpcodesTiming[0x100];

uint8_t* code_ptr_resolve(struct Context *this, uint16_t address);
struct Opcode next_opcode(struct Context *this);

#endif /* opcodes_h */
