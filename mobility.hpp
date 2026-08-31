#pragma once

#include <cstdint>
#include "common.hpp"


// ============================================================
// 合法手生成用マスク
// ============================================================

// 横方向で、隣の行へbitがはみ出すことを防ぐ
constexpr uint64_t LINE_MASK =
    0x7F7F7F7F7F7F7F7FULL;

// 45度回転後の斜め方向用
constexpr uint64_t DIAG9_MASK_RIGHT =
    0x5F6F777B7D7E7F3FULL;

constexpr uint64_t DIAG9_MASK_LEFT =
    0x7D7B776F5F3F7F7EULL;


// ============================================================
// bitboard変換
// ============================================================


// ------------------------------------------------------------
// 左右反転
// ------------------------------------------------------------

inline uint64_t horizontal_mirror(uint64_t x)
{
    x =
        ((x >> 1) & 0x5555555555555555ULL)
        |
        ((x << 1) & 0xAAAAAAAAAAAAAAAAULL);

    x =
        ((x >> 2) & 0x3333333333333333ULL)
        |
        ((x << 2) & 0xCCCCCCCCCCCCCCCCULL);

    return
        ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL)
        |
        ((x << 4) & 0xF0F0F0F0F0F0F0F0ULL);
}


// ------------------------------------------------------------
// 斜め反転
//
// 縦方向の合法手生成に使用する。
// 縦方向を横方向として扱える形へ並べ替える。
// ------------------------------------------------------------

inline uint64_t black_line_mirror(uint64_t x)
{
    uint64_t a;

    a = (x ^ (x >> 9))
        & 0x0055005500550055ULL;

    x = x ^ a ^ (a << 9);


    a = (x ^ (x >> 18))
        & 0x0000333300003333ULL;

    x = x ^ a ^ (a << 18);


    a = (x ^ (x >> 36))
        & 0x000000000F0F0F0FULL;

    return x ^ a ^ (a << 36);
}


// ============================================================
// 45度回転
// ============================================================


// ------------------------------------------------------------
// 45度反時計回りに並べ替え
//
// 本当の画像回転ではなく、
// 同じ斜めライン上のbitを横方向として扱いやすい
// 配置へ並べ替える。
// ------------------------------------------------------------

inline uint64_t rotate_45(uint64_t x)
{
    uint64_t a;

    a = (x ^ (x >> 8))
        & 0x0055005500550055ULL;

    x = x ^ a ^ (a << 8);


    a = (x ^ (x >> 16))
        & 0x0000CC660000CC66ULL;

    x = x ^ a ^ (a << 16);


    a = (x ^ (x >> 32))
        & 0x00000000C3E1F078ULL;

    return x ^ a ^ (a << 32);
}


// ------------------------------------------------------------
// rotate_45() の逆変換
// ------------------------------------------------------------

inline uint64_t unrotate_45(uint64_t x)
{
    uint64_t a;

    a = (x ^ (x >> 32))
        & 0x00000000C3E1F078ULL;

    x = x ^ a ^ (a << 32);


    a = (x ^ (x >> 16))
        & 0x0000CC660000CC66ULL;

    x = x ^ a ^ (a << 16);


    a = (x ^ (x >> 8))
        & 0x0055005500550055ULL;

    return x ^ a ^ (a << 8);
}


// ============================================================
// 135度回転
// ============================================================

inline uint64_t rotate_135(uint64_t x)
{
    uint64_t a;

    a = (x ^ (x >> 8))
        & 0x00AA00AA00AA00AAULL;

    x = x ^ a ^ (a << 8);


    a = (x ^ (x >> 16))
        & 0x0000336600003366ULL;

    x = x ^ a ^ (a << 16);


    a = (x ^ (x >> 32))
        & 0x00000000C3870F1EULL;

    return x ^ a ^ (a << 32);
}


// ------------------------------------------------------------
// rotate_135() の逆変換
// ------------------------------------------------------------

inline uint64_t unrotate_135(uint64_t x)
{
    uint64_t a;

    a = (x ^ (x >> 32))
        & 0x00000000C3870F1EULL;

    x = x ^ a ^ (a << 32);


    a = (x ^ (x >> 16))
        & 0x0000336600003366ULL;

    x = x ^ a ^ (a << 16);


    a = (x ^ (x >> 8))
        & 0x00AA00AA00AA00AAULL;

    return x ^ a ^ (a << 8);
}


// ============================================================
// 横方向の合法手候補生成
// ============================================================

inline uint64_t calc_mobility_line(
    uint64_t player,
    uint64_t opponent
)
{
    // --------------------------------------------------------
    // 一方向
    // --------------------------------------------------------

    uint64_t p1 =
        (player & LINE_MASK) << 1;

    uint64_t result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & LINE_MASK)
        );


    // --------------------------------------------------------
    // 逆方向
    //
    // 左右反転することで、
    // 同じ << 1 の処理を利用する
    // --------------------------------------------------------

    player   = horizontal_mirror(player);
    opponent = horizontal_mirror(opponent);

    p1 =
        (player & LINE_MASK) << 1;


    uint64_t reverse_result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & LINE_MASK)
        );


    return
        result
        |
        horizontal_mirror(reverse_result);
}


// ============================================================
// 45度回転後の斜め方向合法手候補
// ============================================================

inline uint64_t calc_mobility_diag9(
    uint64_t player,
    uint64_t opponent
)
{
    // --------------------------------------------------------
    // 一方向
    // --------------------------------------------------------

    uint64_t p1 =
        (player & DIAG9_MASK_RIGHT) << 1;

    uint64_t result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & DIAG9_MASK_RIGHT)
        );


    // --------------------------------------------------------
    // 逆方向
    // --------------------------------------------------------

    player   = horizontal_mirror(player);
    opponent = horizontal_mirror(opponent);


    p1 =
        (player & DIAG9_MASK_LEFT) << 1;


    uint64_t reverse_result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & DIAG9_MASK_LEFT)
        );


    return
        result
        |
        horizontal_mirror(reverse_result);
}


// ============================================================
// 135度回転後の斜め方向合法手候補
// ============================================================

inline uint64_t calc_mobility_diag7(
    uint64_t player,
    uint64_t opponent
)
{
    // diag9とはマスクの向きが逆

    uint64_t p1 =
        (player & DIAG9_MASK_LEFT) << 1;


    uint64_t result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & DIAG9_MASK_LEFT)
        );


    // --------------------------------------------------------
    // 逆方向
    // --------------------------------------------------------

    player   = horizontal_mirror(player);
    opponent = horizontal_mirror(opponent);


    p1 =
        (player & DIAG9_MASK_RIGHT) << 1;


    uint64_t reverse_result =
        ~(p1 | opponent)
        &
        (
            p1
            +
            (opponent & DIAG9_MASK_RIGHT)
        );


    return
        result
        |
        horizontal_mirror(reverse_result);
}


// ============================================================
// 盤面全体の合法手生成
// ============================================================

inline uint64_t calc_legal(
    uint64_t player,
    uint64_t opponent
)
{
    // --------------------------------------------------------
    // 横
    // --------------------------------------------------------

    uint64_t horizontal =
        calc_mobility_line(
            player,
            opponent
        );


    // --------------------------------------------------------
    // 縦
    //
    // black_line_mirrorで並べ替えて
    // 横方向として計算する
    // --------------------------------------------------------

    uint64_t vertical =
        black_line_mirror(
            calc_mobility_line(
                black_line_mirror(player),
                black_line_mirror(opponent)
            )
        );


    // --------------------------------------------------------
    // 斜め方向1
    // --------------------------------------------------------

    uint64_t diagonal9 =
        unrotate_45(
            calc_mobility_diag9(
                rotate_45(player),
                rotate_45(opponent)
            )
        );


    // --------------------------------------------------------
    // 斜め方向2
    // --------------------------------------------------------

    uint64_t diagonal7 =
        unrotate_135(
            calc_mobility_diag7(
                rotate_135(player),
                rotate_135(opponent)
            )
        );


    // 4方向の候補をまとめる
    uint64_t result =
        horizontal
        |
        vertical
        |
        diagonal9
        |
        diagonal7;


    // player/opponentの石が存在するマスを除外
    // → 最終的に空きマスだけを合法手として残す
    return result & ~(player | opponent);
}