#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>

#include "definitions.h"
#include "debug.h"
#include "joypad.h"

#ifdef ENABLE_JOYSTICK
SDL_Joystick *joy;
#endif

void joypad_init() {
#ifdef ENABLE_JOYSTICK
    if (SDL_NumJoysticks() == 0) {
        fprintf(stderr, "No joysticks available!\n");
        exit(EXIT_FAILURE);
    }

    joy = SDL_JoystickOpen(0);
    if (!joy) {
        fprintf(stderr, "Can't open joystick: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    printf("Joystick Name: %s\n", SDL_JoystickName(joy));
    printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
    printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
    printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
    printf("Number of Hats: %d\n", SDL_JoystickNumHats(joy));
#endif
}

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
    uint8_t previous = this->joypad.keys.raw;

#ifdef ENABLE_JOYSTICK
    if (SDL_JoystickNumHats(joy) > 0) {
        const Uint8 joy_value = SDL_JoystickGetHat(joy, 0);
        this->joypad.keys.down  = (joy_value & SDL_HAT_DOWN ) ? 1 : 0;
        this->joypad.keys.up    = (joy_value & SDL_HAT_UP   ) ? 1 : 0;
        this->joypad.keys.left  = (joy_value & SDL_HAT_LEFT ) ? 1 : 0;
        this->joypad.keys.right = (joy_value & SDL_HAT_RIGHT) ? 1 : 0;
    } else {
        this->joypad.keys.down  = SDL_JoystickGetButton(joy, 12) ? 1 : 0;
        this->joypad.keys.up    = SDL_JoystickGetButton(joy, 11) ? 1 : 0;
        this->joypad.keys.left  = SDL_JoystickGetButton(joy, 13) ? 1 : 0;
        this->joypad.keys.right = SDL_JoystickGetButton(joy, 14) ? 1 : 0;
    }

    this->joypad.keys.buttonA = SDL_JoystickGetButton(joy, 1) ? 1 : 0;
    this->joypad.keys.buttonB = SDL_JoystickGetButton(joy, 0) ? 1 : 0;
    this->joypad.keys.select  = (SDL_JoystickGetButton(joy, 6) || SDL_JoystickGetButton(joy, 9) ) ? 1 : 0;
    this->joypad.keys.start   = (SDL_JoystickGetButton(joy, 7) || SDL_JoystickGetButton(joy, 8) ) ? 1 : 0;
#else
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    this->joypad.keys.down = state[SDL_SCANCODE_DOWN] ? 1 : 0;
    this->joypad.keys.up = state[SDL_SCANCODE_UP] ? 1 : 0;
    this->joypad.keys.left = state[SDL_SCANCODE_LEFT] ? 1 : 0;
    this->joypad.keys.right = state[SDL_SCANCODE_RIGHT] ? 1 : 0;
    this->joypad.keys.buttonA = state[SDL_SCANCODE_Z] ? 1 : 0;
    this->joypad.keys.buttonB = state[SDL_SCANCODE_A] ? 1 : 0;
    this->joypad.keys.select = state[SDL_SCANCODE_1] ? 1 : 0;
    this->joypad.keys.start = state[SDL_SCANCODE_2] ? 1 : 0;
#endif

    if (previous != this->joypad.keys.raw) {
        this->interrupts.IF.joypad = 1;
    }
}
