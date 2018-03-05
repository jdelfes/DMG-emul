#ifndef debug_h
#define debug_h

#define d_printf(...)
//#define d_printf(...) fprintf(stderr, __VA_ARGS__)

void print_debug(const struct Context *this, int failure) __attribute__((noreturn));

#endif /* debug_h */
