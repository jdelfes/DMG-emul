#ifndef joypad_h
#define joypad_h

bool joypad_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool joypad_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void joypad_check(struct Context *this);

#endif /* joypad_h */
