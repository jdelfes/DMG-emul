#ifndef video_h
#define video_h

void video_init(void);
bool video_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool video_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void video_check(struct Context *this);

#endif /* video_h */
