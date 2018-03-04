#ifndef sound_h
#define sound_h

bool sound_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool sound_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* sound_h */
