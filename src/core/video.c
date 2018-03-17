#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL.h>

#include "definitions.h"
#include "memory.h"
#include "debug.h"
#include "sound.h"
#include "video.h"

static void render_line(struct Context *this, uint16_t line_number);
static uint8_t get_pixel_data(struct Context *this, const uint8_t *tile_data, uint8_t x, uint8_t y);
static void set_pixel(struct Context *this, uint8_t x, uint8_t y, uint8_t bit_color);
static void update_frame(struct Context *this);

#ifdef ENABLE_VIDEO
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
#endif

void video_init() {
#ifdef ENABLE_VIDEO
    window = SDL_CreateWindow("DMG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              160 * 3, 144 * 3, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } else {
        renderer = SDL_CreateRenderer(window, -1,
#ifdef ENABLE_VSYNC
                                      SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
#else
                                      SDL_RENDERER_SOFTWARE);
#endif
        if (!renderer) {
            fprintf(stderr, "Couldn't create renderer! SDL_Error: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STATIC, 160, 144);
        if (!texture) {
            fprintf(stderr, "Couldn't create texture! SDL_Error: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
#endif
}

bool video_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff40: // LCDC - LCD Control (R/W)
            *ret_value = this->video.LCDC.raw;
            return true;
        case 0xff41: // STAT - LCDC Status (R/W)
            this->video.STAT.coincidence_flag = (this->video.LY == this->video.LYC) ? 1 : 0;
            *ret_value = (this->video.STAT.raw & ~(0x03))
                | ((this->video.LY >= 144) ? 1 : this->video.STAT.mode_flag)
                | 0x80;
            return true;
        case 0xff42: // SCY - Scroll Y (R/W)
            *ret_value = this->video.SCY;
            return true;
        case 0xff43: // SCX - Scroll X (R/W)
            *ret_value = this->video.SCX;
            return true;
        case 0xff44: // LY - LCDC Y-Coordinate (R)
            *ret_value = this->video.LY;
            return true;
        case 0xff45: // LYC - LY Compare (R/W)
            *ret_value = this->video.LYC;
            return true;
        case 0xff47: // BGP - BG Palette Data (R/W) - Non CGB Mode Only
            *ret_value = this->video.BGP.raw;
            return true;
        case 0xff48: // OBP0 - Object Palette 0 Data (R/W) - Non CGB Mode Only
            *ret_value = this->video.OBP0.raw;
            return true;
        case 0xff49: // OBP1 - Object Palette 1 Data (R/W) - Non CGB Mode Only
            *ret_value = this->video.OBP1.raw;
            return true;
        case 0xff4a: // WY - Window Y Position (R/W)
            *ret_value = this->video.WY;
            return true;
        case 0xff4b: // WX - Window X Position minus 7 (R/W)
            *ret_value = this->video.WX;
            return true;
    }
    return false;
}

bool video_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    bool start_lcd;
    switch (address) {
        case 0xff40: // LCDC - LCD Control (R/W)
            d_printf("LCDC %02x\n", value);
            start_lcd = !this->video.LCDC.lcd_display_enable;
            this->video.LCDC.raw = value;
            start_lcd &= this->video.LCDC.lcd_display_enable;

            if (start_lcd) {
                this->video.LY = 0;
                this->video.last_cpu_timing = this->cpu_timing;
                this->video.timing = 0;
                this->video.STAT.mode_flag = 2;
                memset(this->video.screen, 0, sizeof(this->video.screen));
            }
            return true;
        case 0xff41: // STAT - LCDC Status (R/W)
            d_printf("STAT %02x\n", value);
            this->video.STAT.raw = (value & ~0x03) | this->video.STAT.mode_flag;
            return true;
        case 0xff42: // SCY - Scroll Y (R/W)
            d_printf("SCY %02x\n", value);
            this->video.SCY = value;
            return true;
        case 0xff43: // SCX - Scroll X (R/W)
            d_printf("SCX %02x\n", value);
            this->video.SCX = value;
            return true;
        case 0xff44: // LY - LCDC Y-Coordinate (R)
            d_printf("LY %02x\n", value);
            return true;
        case 0xff45: // LYC - LY Compare (R/W)
            d_printf("LYC %02x\n", value);
            this->video.LYC = value;
            return true;
        case 0xff46: // DMA - DMA Transfer and Start Address (R/W)
            d_printf("DMA: %02x\n", value);
            if (value > 0xf1) {
                fprintf(stderr, "wrong DMA value %02x\n", value);
                exit(EXIT_FAILURE);
            }
            for (uint16_t i = 0; i < sizeof(this->oam); i++) {
                this->oam[i] = get_mem_u8(this, (value << 8) | i);
            }
            return true;
        case 0xff47: // BGP - BG Palette Data (R/W) - Non CGB Mode Only
            d_printf("BGP %02x\n", value);
            this->video.BGP.raw = value;
            return true;
        case 0xff48: // OBP0 - Object Palette 0 Data (R/W) - Non CGB Mode Only
            d_printf("OBP0 %02x\n", value);
            this->video.OBP0.raw = value;
            return true;
        case 0xff49: // OBP1 - Object Palette 1 Data (R/W) - Non CGB Mode Only
            d_printf("OBP1 %02x\n", value);
            this->video.OBP1.raw = value;
            return true;
        case 0xff4a: // WY - Window Y Position (R/W)
            d_printf("WY %02x\n", value);
            this->video.WY = value;
            return true;
        case 0xff4b: // WX - Window X Position minus 7 (R/W)
            d_printf("WX %02x\n", value);
            this->video.WX = value;
            return true;
    }

    return false;
}

void video_check(struct Context *this) {
    if (!this->video.LCDC.lcd_display_enable) {
        this->video.LY = 0;
        this->video.STAT.mode_flag = 0;
        return;
    }

    this->video.timing += this->cpu_timing - this->video.last_cpu_timing;
    this->video.last_cpu_timing = this->cpu_timing;

    uint16_t needed;
    switch (this->video.STAT.mode_flag) {
        case 0: // 204 - hblank
            needed = 204;
            break;
//            case 1: // 4560 - vblank
//                break;
        case 2: // 80 - oam
            needed = 80;
            break;
        case 3: // 172 - vram
            needed = 172;
            break;
        default:
            exit(EXIT_FAILURE);
    }

    if (this->video.timing >= needed) {
        this->video.timing -= needed;
        switch (this->video.STAT.mode_flag) {
            case 0: // 204 - hblank to oam
                this->video.STAT.mode_flag = 2;
                this->video.LY++;
                if (this->video.LY == 144) {
                    this->interrupts.IF.v_blank = 1;
                    if (this->video.STAT.mode_1_v_blank_interrupt) {
                        this->interrupts.IF.lcd_stat = 1;
                    }
                }
                if (this->video.STAT.mode_2_oam_interrupt) {
                    this->interrupts.IF.lcd_stat = 1;
                }
                if (this->video.LY == 154) {
                    this->video.LY = 0;
                }
                if (this->video.STAT.lyc_ly_coincidence_interrupt &&
                    this->video.LYC == this->video.LY) {
                    this->interrupts.IF.lcd_stat = 1;
                }
                break;
            case 2: // 80 - oam to vram
                this->video.STAT.mode_flag = 3;
                if (this->video.LY < 144) {
                    render_line(this, this->video.LY);
                }
                break;
            case 3: // 172 - vram to hblank
                this->video.STAT.mode_flag = 0;
                if (this->video.STAT.mode_0_h_blank_interrupt) {
                    this->interrupts.IF.lcd_stat = 1;
                }
                break;
        }
    }
}

static uint8_t get_pixel_data(struct Context *this,const uint8_t *tile_data, uint8_t x, uint8_t y) {
#ifdef ENABLE_STRICT
    if (x >= 8 || y >= (this->video.LCDC.obj_size ? 16 : 8)) {
        fprintf(stderr, "wrong pixel pos %i x %i\n", x, y);
        exit(EXIT_FAILURE);
    }
#endif
    uint8_t pixel_data0 = tile_data[y * 2];
    uint8_t pixel_data1 = tile_data[y * 2 + 1];
    return ((pixel_data0 & (0x80 >> x)) ? 1 : 0) |
        ((pixel_data1 & (0x80 >> x)) ? 2 : 0);
}

static void set_pixel(struct Context *this, uint8_t x, uint8_t y, uint8_t bit_color) {
    uint8_t gray_level = 0xff - (bit_color * 85);
    this->video.screen[y][x].a = 0xff;
    this->video.screen[y][x].r = gray_level;
    this->video.screen[y][x].g = gray_level;
    this->video.screen[y][x].b = gray_level;
}

static void render_line(struct Context *this, uint16_t line_number) {
    // render background
    if (this->video.LCDC.bg_window_display_priority) {
        uint8_t pos_y = line_number + this->video.SCY;
        uint8_t *bg_tile_map = this->vram +
            (this->video.LCDC.bg_tile_map_display_select ? 0x1c00 : 0x1800);
        uint8_t *tile_data = this->vram +
            (this->video.LCDC.bg_window_tile_data_select ? 0x0000 : 0x0800);
        uint8_t tile_y = pos_y / 8;
        uint8_t tile_pixel_y = pos_y % 8;
        for (uint8_t x = 0; x < 160; x++) {
            uint8_t pos_x = x + this->video.SCX;
            uint8_t tile_x = pos_x / 8;
            uint8_t tile_pixel_x = pos_x % 8;

            uint8_t tile_number = bg_tile_map[(tile_y * 32) + tile_x];
            if (!this->video.LCDC.bg_window_tile_data_select) {
                tile_number += 128;
            }
            uint8_t *selected_tile_data = tile_data + (tile_number * 16); // 2 bytes per row, 8x8 pixels
            uint8_t pixel_data = get_pixel_data(this, selected_tile_data, tile_pixel_x, tile_pixel_y);

            pixel_data = (this->video.BGP.raw >> (pixel_data * 2)) & 0x03;

            set_pixel(this, x, line_number, pixel_data);
        }
    }

    // render window
    if (this->video.LCDC.window_display_enable && (line_number >= this->video.WY)) {
        uint8_t pos_y = line_number - this->video.WY;
        uint8_t *window_tile_map = this->vram +
            (this->video.LCDC.window_tile_map_display_select ? 0x1c00 : 0x1800);
        uint8_t *tile_data = this->vram +
            (this->video.LCDC.bg_window_tile_data_select ? 0x0000 : 0x0800);
        uint8_t tile_y = pos_y / 8;
        uint8_t tile_pixel_y = pos_y % 8;
        uint8_t min_x = this->video.WX >= 7 ? this->video.WX - 7 : 0;
        for (uint8_t x = min_x; x < 160; x++) {
            uint8_t pos_x = x - this->video.WX + 7;
            uint8_t tile_x = pos_x / 8;
            uint8_t tile_pixel_x = pos_x % 8;

            uint8_t tile_number = window_tile_map[(tile_y * 32) + tile_x];
            if (!this->video.LCDC.bg_window_tile_data_select) {
                tile_number += 128;
            }
            uint8_t *selected_tile_data = tile_data + (tile_number * 16); // 2 bytes per row, 8x8 pixels
            uint8_t pixel_data = get_pixel_data(this, selected_tile_data, tile_pixel_x, tile_pixel_y);

            pixel_data = (this->video.BGP.raw >> (pixel_data * 2)) & 0x03;

            set_pixel(this, x, line_number, pixel_data);
        }
    }

    // render sprites
    if (this->video.LCDC.obj_display_enable) {
        const Sprite *visible_sprites[sizeof(this->sprites)];
        uint8_t sprite_height = this->video.LCDC.obj_size ? 16 : 8;
        uint8_t visibles = 0;
        for (uint8_t i = 0; i < (sizeof(this->sprites) / sizeof(this->sprites[0])); i++) {
            const Sprite *sprite = &this->sprites[i];
            if (sprite->x_pos != 0 &&
                (line_number + 16) >= sprite->y_pos &&
                (line_number + 16) < (sprite->y_pos + sprite_height)) {
                visible_sprites[visibles++] = sprite;
            }
        }

        if (visibles > 0) {
            if (visibles > 10) {
                visibles = 10;
            }

            if (visibles > 1) {
                for (int i = 0; i < (visibles - 1); i++) {
                    for (int j = i + 1; j < visibles; j++) {
                        if (visible_sprites[i]->x_pos > visible_sprites[j]->x_pos) {
                            const Sprite *p = visible_sprites[i];
                            visible_sprites[i] = visible_sprites[j];
                            visible_sprites[j] = p;
                        }
                    }
                }
            }
            for (uint16_t x = 0; x < 160; x++) {
                for (uint8_t i = 0; i < visibles; i++) {
                    const Sprite *sprite = visible_sprites[i];
                    int diff_x = x - (sprite->x_pos - 8);
                    if (diff_x >= 0 && diff_x < 8) {
                        if (sprite->flags.obj_priority &&
                            this->video.screen[line_number][x].r != 0xff) {
                            continue;
                        }

                        uint8_t *tile_data = this->vram + (sprite->tile_number * 16);
                        uint8_t diff_y = line_number - (sprite->y_pos - 16);
                        uint8_t pixel_x = sprite->flags.x_flip ? (7 - diff_x) : diff_x;
                        uint8_t pixel_y = sprite->flags.y_flip ? (sprite_height - 1 - diff_y) : diff_y;
                        uint8_t pixel_data = get_pixel_data(this, tile_data, pixel_x, pixel_y);

                        if (pixel_data == 0) {
                            // transparent, keep background color
                            continue;
                        }

                        pixel_data = ((sprite->flags.palette_number ? this->video.OBP1.raw : this->video.OBP0.raw)
                            >> (pixel_data * 2)) & 0x03;

                        set_pixel(this, x, line_number, pixel_data);
                        break;
                    }
                }
            }
        }
    }

    if (line_number == 143) {
        update_frame(this);
    }
}

static void update_frame(struct Context *this) {
#ifdef ENABLE_VIDEO
    SDL_UpdateTexture(texture, NULL, this->video.screen, sizeof(this->video.screen[0]));
    SDL_RenderClear(renderer);
    SDL_Rect rect = {0};
    SDL_GetWindowSize(window, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);
    snd_card_flush(this);
    memset(this->video.screen, 0xff, sizeof(this->video.screen));

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
        }
    }
#endif
}
