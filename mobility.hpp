#pragma once

#include <cstdint>
#include "common.hpp"


// ============================================================
// bitboard変換
// ============================================================

inline uint64_t horizontal_mirror(uint64_t x) {
    x = ((x >> 1) & 0x5555555555555555ULL)
      | ((x << 1) & 0xAAAAAAAAAAAAAAAAULL);

    x = ((x >> 2) & 0x3333333333333333ULL)
      | ((x << 2) & 0xCCCCCCCCCCCCCCCCULL);

    return ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL)
         | ((x << 4) & 0xF0F0F0F0F0F0F0F0ULL);
}


inline uint64_t black_line_mirror(uint64_t x) {
    uint64_t a = (x ^ (x >> 9)) & 0x0055005500550055ULL;
    x = x ^ a ^ (a << 9);

    a = (x ^ (x >> 18)) & 0x0000333300003333ULL;
    x = x ^ a ^ (a << 18);

    a = (x ^ (x >> 36)) & 0x000000000F0F0F0FULL;

    return x ^ a ^ (a << 36);
}


inline uint64_t rotate_45(uint64_t x) {
    uint64_t a = (x ^ (x >> 8)) & 0x0055005500550055ULL;
    x = x ^ a ^ (a << 8);

    a = (x ^ (x >> 16)) & 0x0000CC660000CC66ULL;
    x = x ^ a ^ (a << 16);

    a = (x ^ (x >> 32)) & 0x00000000C3E1F078ULL;

    return x ^ a ^ (a << 32);
}


inline uint64_t unrotate_45(uint64_t x) {
    uint64_t a = (x ^ (x >> 32)) & 0x00000000C3E1F078ULL;
    x = x ^ a ^ (a << 32);

    a = (x ^ (x >> 16)) & 0x0000CC660000CC66ULL;
    x = x ^ a ^ (a << 16);

    a = (x ^ (x >> 8)) & 0x0055005500550055ULL;

    return x ^ a ^ (a << 8);
}


inline uint64_t rotate_135(uint64_t x) {
    uint64_t a = (x ^ (x >> 8)) & 0x00AA00AA00AA00AAULL;
    x = x ^ a ^ (a << 8);

    a = (x ^ (x >> 16)) & 0x0000336600003366ULL;
    x = x ^ a ^ (a << 16);

    a = (x ^ (x >> 32)) & 0x00000000C3870F1EULL;

    return x ^ a ^ (a << 32);
}


inline uint64_t unrotate_135(uint64_t x) {
    uint64_t a = (x ^ (x >> 32)) & 0x00000000C3870F1EULL;
    x = x ^ a ^ (a << 32);

    a = (x ^ (x >> 16)) & 0x0000336600003366ULL;
    x = x ^ a ^ (a << 16);

    a = (x ^ (x >> 8)) & 0x00AA00AA00AA00AAULL;

    return x ^ a ^ (a << 8);
}


// ============================================================
// 合法手生成
// ============================================================

inline uint64_t calc_mobility_line(uint64_t p, uint64_t o) {
    uint64_t p1 =
        (p & 0x7F7F7F7F7F7F7F7FULL) << 1;

    uint64_t res =
        ~(p1 | o)
        &
        (p1 + (o & 0x7F7F7F7F7F7F7F7FULL));

    p = horizontal_mirror(p);
    o = horizontal_mirror(o);

    p1 =
        (p & 0x7F7F7F7F7F7F7F7FULL) << 1;

    return res
        |
        horizontal_mirror(
            ~(p1 | o)
            &
            (p1 + (o & 0x7F7F7F7F7F7F7F7FULL))
        );
}


inline uint64_t calc_mobility_diag9(uint64_t p, uint64_t o) {
    uint64_t p1 =
        (p & 0x5F6F777B7D7E7F3FULL) << 1;

    uint64_t res =
        ~(p1 | o)
        &
        (p1 + (o & 0x5F6F777B7D7E7F3FULL));

    p = horizontal_mirror(p);
    o = horizontal_mirror(o);

    p1 =
        (p & 0x7D7B776F5F3F7F7EULL) << 1;

    return res
        |
        horizontal_mirror(
            ~(p1 | o)
            &
            (p1 + (o & 0x7D7B776F5F3F7F7EULL))
        );
}


inline uint64_t calc_mobility_diag7(uint64_t p, uint64_t o) {
    uint64_t p1 =
        (p & 0x7D7B776F5F3F7F7EULL) << 1;

    uint64_t res =
        ~(p1 | o)
        &
        (p1 + (o & 0x7D7B776F5F3F7F7EULL));

    p = horizontal_mirror(p);
    o = horizontal_mirror(o);

    p1 =
        (p & 0x5F6F777B7D7E7F3FULL) << 1;

    return res
        |
        horizontal_mirror(
            ~(p1 | o)
            &
            (p1 + (o & 0x5F6F777B7D7E7F3FULL))
        );
}


inline uint64_t calc_legal(uint64_t p, uint64_t o) {
    uint64_t res =
        calc_mobility_line(p, o)

        |

        black_line_mirror(
            calc_mobility_line(
                black_line_mirror(p),
                black_line_mirror(o)
            )
        )

        |

        unrotate_45(
            calc_mobility_diag9(
                rotate_45(p),
                rotate_45(o)
            )
        )

        |

        unrotate_135(
            calc_mobility_diag7(
                rotate_135(p),
                rotate_135(o)
            )
        );

    return res & ~(p | o);
}