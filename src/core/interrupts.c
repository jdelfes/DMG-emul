#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "timer.h"
#include "opcodes.h"
#include "debug.h"
#include "interrupts.h"

static void interrupt_service(struct Context *this, uint16_t address) {
    this->interrupts.IME = false;
    push16(this, this->cpu.registers.PC);
    this->cpu.registers.PC = address;
    this->cpu_halted = false;
    for (int i = 0; i < 20; i += 4) {
        this->cpu_timing += 4;
        timer_check(this);
        sound_tick(this);
    }
}

bool interrupts_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff0f: // IF - Interrupt Flag (R/W)
            *ret_value = this->interrupts.IF.raw | 0xe0;
            return true;
        case 0xffff: // IE - Interrupt Enable (R/W)
            *ret_value = this->interrupts.IE.raw;
            return true;
    }

    return false;
}

bool interrupts_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff0f: // IF - Interrupt Flag (R/W)
            d_printf("IF %02x\n", value);
            this->interrupts.IF.raw = value & 0x1f;
            return true;
        case 0xffff: // IE - Interrupt Enable (R/W)
            d_printf("IE %02x\n", value);
            this->interrupts.IE.raw = value;
            return true;
    }
    return false;
}

void interrupts_check(struct Context *this) {
    if (this->interrupts.enable_interrupts_step > 0 &&
        --this->interrupts.enable_interrupts_step == 0) {
        this->interrupts.IME = true;
    }

    if (this->interrupts.IME) {
        if (this->interrupts.IE.v_blank && this->interrupts.IF.v_blank) {
            d_printf("vblank int\n");
            this->interrupts.IF.v_blank = 0;
            interrupt_service(this, 0x0040);
        } else if (this->interrupts.IE.lcd_stat && this->interrupts.IF.lcd_stat) {
            d_printf("lcd int\n");
            this->interrupts.IF.lcd_stat = 0;
            interrupt_service(this, 0x0048);
        } else if (this->interrupts.IE.timer && this->interrupts.IF.timer) {
            d_printf("timer int\n");
            this->interrupts.IF.timer = 0;
            interrupt_service(this, 0x0050);
        } else if (this->interrupts.IE.serial && this->interrupts.IF.serial) {
            d_printf("serial int\n");
            this->interrupts.IF.serial = 0;
            interrupt_service(this, 0x0058);
        } else if (this->interrupts.IE.joypad && this->interrupts.IF.joypad) {
            d_printf("joypad int\n");
            this->interrupts.IF.joypad = 0;
            interrupt_service(this, 0x0060);
        }
        return;
    }

    if (this->cpu_halted && (this->interrupts.IE.raw & this->interrupts.IF.raw & 0x1f)) {
        this->cpu_halted = false;
    }
}
