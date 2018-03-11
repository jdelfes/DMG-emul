#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "snd_duty.h"

float duty00[] = {-1,  1,  1,  1,  1,  1, 1, 1};
float duty01[] = {-1, -1,  1,  1,  1,  1, 1, 1};
float duty10[] = {-1, -1, -1, -1,  1,  1, 1, 1};
float duty11[] = {-1, -1, -1, -1, -1, -1, 1, 1};

float snd_duty_value(uint8_t duty, uint8_t step) {
    float *duty_ptr = NULL;
    switch (duty) {
        case 0: duty_ptr = duty00; break;
        case 1: duty_ptr = duty01; break;
        case 2: duty_ptr = duty10; break;
        case 3: duty_ptr = duty11; break;
        default: exit(EXIT_FAILURE);
    }
    return duty_ptr[step];
}
