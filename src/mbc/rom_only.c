#include "definitions.h"
#include "rom_only.h"

void rom_only_init(struct Context *this) {
#ifdef STRICT_MODE
    if (this->rom_size != 0x8000) {
        fprintf(stderr, "Weird rom size for ROM ONLY mode: %llu\n", this->rom_size);
        exit(EXIT_FAILURE);
    }
#endif
}

bool rom_only_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value) {
    if (address < 0x8000) {
        *ret_value = this->rom_data[address];
        return true;
    }

    return false;
}

bool rom_only_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    return false;
}
