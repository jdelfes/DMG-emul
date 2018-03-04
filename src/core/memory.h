#ifndef memory_h
#define memory_h

#include <stdio.h>

uint8_t get_mem_u8(struct Context *this, uint16_t address);
uint16_t get_mem_u16(struct Context *this, uint16_t address);
void set_mem_u8(struct Context *this, uint16_t address, uint8_t value);
void set_mem_u16(struct Context *this, uint16_t address, uint16_t value);

#endif /* memory_h */
