#ifndef rom_only_h
#define rom_only_h

void rom_only_init(struct Context *this);
bool rom_only_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value);
bool rom_only_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* rom_only_h */
