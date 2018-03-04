#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>

#include "definitions.h"
#include "debug.h"
#include "joypad.h"

bool joypad_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff00: // P1/JOYP - Joypad (R/W)
            *ret_value = ((this->joypad.JOYP & 0x20) ? 0 : (~this->joypad.keys.buttons & 0x0f)) |
                ((this->joypad.JOYP & 0x10) ? 0 : (~this->joypad.keys.directions & 0x0f)) |
                0xc0;
            return true;
    }
    return false;
}

bool joypad_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff00: // P1/JOYP - Joypad (R/W)
            d_printf("JOYP: %02x\n", value);
            this->joypad.JOYP = (value & 0xf0) | (this->joypad.JOYP & 0x0f);
            return true;
    }

    return false;
}

void joypad_check(struct Context *this) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    uint8_t previous = this->joypad.keys.raw;

    this->joypad.keys.down = state[SDL_SCANCODE_DOWN] ? 1 : 0;
    this->joypad.keys.up = state[SDL_SCANCODE_UP] ? 1 : 0;
    this->joypad.keys.left = state[SDL_SCANCODE_LEFT] ? 1 : 0;
    this->joypad.keys.right = state[SDL_SCANCODE_RIGHT] ? 1 : 0;
    this->joypad.keys.buttonA = state[SDL_SCANCODE_Z] ? 1 : 0;
    this->joypad.keys.buttonB = state[SDL_SCANCODE_A] ? 1 : 0;
    this->joypad.keys.select = state[SDL_SCANCODE_1] ? 1 : 0;
    this->joypad.keys.start = state[SDL_SCANCODE_2] ? 1 : 0;

    if (previous != this->joypad.keys.raw) {
        this->interrupts.IF.joypad = 1;
    }
}
