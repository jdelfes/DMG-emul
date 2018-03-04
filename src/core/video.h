#ifndef video_h
#define video_h

struct __attribute__((packed)) Pixel {
    union {
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
        uint32_t raw;
    };
};

struct Video {
    uint64_t last_cpu_timing;
    uint64_t timing;
    struct Pixel screen[144][160];
    union {
        uint8_t raw;
        struct {
            uint8_t mode_flag: 2;
            uint8_t coincidence_flag: 1;
            uint8_t mode_0_h_blank_interrupt: 1;
            uint8_t mode_1_v_blank_interrupt: 1;
            uint8_t mode_2_oam_interrupt: 1;
            uint8_t lyc_ly_coincidence_interrupt: 1;
        };
    } STAT;
    union {
        uint8_t raw;
        struct {
            uint8_t bg_window_display_priority: 1;
            uint8_t obj_display_enable: 1;
            uint8_t obj_size: 1;
            uint8_t bg_tile_map_display_select: 1;
            uint8_t bg_window_tile_data_select: 1;
            uint8_t window_display_enable: 1;
            uint8_t window_tile_map_display_select: 1;
            uint8_t lcd_display_enable: 1;
        };
    } LCDC;
    uint8_t SCY;
    uint8_t SCX;
    uint8_t LY;
    uint8_t LYC;
    union {
        uint8_t raw;
        union {
            uint8_t shade_color_number0: 2;
            uint8_t shade_color_number1: 2;
            uint8_t shade_color_number2: 2;
            uint8_t shade_color_number3: 2;
        };
    } BGP;
    union {
        uint8_t raw;
        union {
            uint8_t : 2;
            uint8_t shade_color_number1: 2;
            uint8_t shade_color_number2: 2;
            uint8_t shade_color_number3: 2;
        };
    } OBP0;
    union {
        uint8_t raw;
        union {
            uint8_t : 2;
            uint8_t shade_color_number1: 2;
            uint8_t shade_color_number2: 2;
            uint8_t shade_color_number3: 2;
        };
    } OBP1;
    uint8_t WY;
    uint8_t WX;
};

void video_init(void);
bool video_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool video_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);
void video_check(struct Context *this);

#endif /* video_h */
