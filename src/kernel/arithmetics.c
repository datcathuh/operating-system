#include "types.h"

int64_t __divdi3(int64_t a, int64_t b) {
    if (b == 0) {
        // Handle divide by zero (your OS can panic, halt, etc.)
        return 0;
    }

    // Track sign
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = !neg;
    }
    if (b < 0) {
        b = -b;
        neg = !neg;
    }

    uint64_t ua = (uint64_t)a;
    uint64_t ub = (uint64_t)b;
    uint64_t q = 0;
    uint64_t r = 0;

    // Basic restoring division algorithm
    for (int i = 63; i >= 0; i--) {
        r <<= 1;
        r |= (ua >> i) & 1;
        if (r >= ub) {
            r -= ub;
            q |= (1ULL << i);
        }
    }

    int64_t res = (int64_t)q;
    return neg ? -res : res;
}
