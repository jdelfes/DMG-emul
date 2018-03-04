#ifndef serial_h
#define serial_h

bool serial_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool serial_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* serial_h */
