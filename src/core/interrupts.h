#ifndef interrupts_h
#define interrupts_h

bool interrupts_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool interrupts_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void interrupts_check(struct Context *this);

#endif /* interrupts_h */
