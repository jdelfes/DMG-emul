#ifndef timer_h
#define timer_h

bool timer_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool timer_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void timer_check(struct Context *this);

#endif /* timer_h */
