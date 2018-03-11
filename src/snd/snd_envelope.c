#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "snd_envelope.h"

float snd_envelope_output_level_mult(uint8_t output_level) {
//    return output_level / 15.0;
    switch (output_level) {
        case  0: return 0.000;
        case  1: return 0.067;
        case  2: return 0.133;
        case  3: return 0.200;
        case  4: return 0.267;
        case  5: return 0.333;
        case  6: return 0.400;
        case  7: return 0.467;
        case  8: return 0.533;
        case  9: return 0.600;
        case 10: return 0.667;
        case 11: return 0.733;
        case 12: return 0.800;
        case 13: return 0.867;
        case 14: return 0.933;
        case 15: return 1.000;
        default: exit(EXIT_FAILURE);
    }
}
