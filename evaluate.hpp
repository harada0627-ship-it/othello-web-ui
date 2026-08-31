#pragma once

#include <cstdint>

#include "board.hpp"
#include "common.hpp"

using namespace std;


// ============================================================
// 評価関数用定数
// ============================================================

constexpr int SC_W = 64;
constexpr int C_KAIHOU = -2;
constexpr int C_LEGAL = 7;
constexpr int C_KAKUTEI = 1;



// ============================================================
// マス評価値
//
// i = 0  → A1
// i = 63 → H8
//
// bitboard上では
// bit63 → A1
// bit0  → H8
//
// なので (63 - i) を使用
// ============================================================

inline int cell_weight[HW2] = {

     30, -12,  0, -1, -1,  0, -12,  30,
    -12, -15, -3, -3, -3, -3, -15, -12,
      0,  -3,  0, -1, -1,  0,  -3,   0,
     -1,  -3, -1, -1, -1, -1,  -3,  -1,
     -1,  -3, -1, -1, -1, -1,  -3,  -1,
      0,  -3,  0, -1, -1,  0,  -3,   0,
    -12, -15, -3, -3, -3, -3, -15, -12,
     30, -12,  0, -1, -1,  0, -12,  30
};


// ============================================================
// 1方向隣のマスを取得
//
// 以前の board_bit.hpp の make_mask() と同じ処理
// ============================================================

inline uint64_t make_mask(
    uint64_t board,
    int direction
) {

    switch (direction) {

    case 0:     // 上
        return
            (board & 0x00FFFFFFFFFFFFFFULL)
            << 8;

    case 1:     // 右上
        return
            (board & 0x00FEFEFEFEFEFEFEULL)
            << 7;

    case 2:     // 右
        return
            (board & 0xFEFEFEFEFEFEFEFEULL)
            >> 1;

    case 3:     // 右下
        return
            (board & 0xFEFEFEFEFEFEFE00ULL)
            >> 9;

    case 4:     // 下
        return
            (board & 0xFFFFFFFFFFFFFF00ULL)
            >> 8;

    case 5:     // 左下
        return
            (board & 0x7F7F7F7F7F7F7F00ULL)
            >> 7;

    case 6:     // 左
        return
            (board & 0x7F7F7F7F7F7F7F7FULL)
            << 1;

    case 7:     // 左上
        return
            (board & 0x007F7F7F7F7F7F7FULL)
            << 9;

    default:
        return 0ULL;
    }
}


// ============================================================
// 開放度
//
// stones に隣接している空きマスをbitboardで返す
// ============================================================

inline uint64_t calc_kaihou(
    uint64_t stones,
    uint64_t player,
    uint64_t opponent
) {

    uint64_t kaihou_board = 0;

    uint64_t vacant_board =
        ~(player | opponent);


    for (int i = 0; i < 8; ++i) {

        uint64_t mask =
            make_mask(
                stones,
                i
            );

        kaihou_board |=(  mask & vacant_board );
    }


    return kaihou_board;
}
 // --------------------------------------------------------
    // 確定石
    // --------------------------------------------------------

    inline int calc_kakutei(uint64_t stones) {
    uint64_t kakutei = 0ULL;

    if (stones & (1ULL << 63)) {
        kakutei |= (1ULL << 63);
        for (int bit = 62; bit >= 56; --bit) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
        for (int bit = 55; bit >= 7; bit -= 8) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
    }

    if (stones & (1ULL << 56)) {
        kakutei |= (1ULL << 56);
        for (int bit = 57; bit <= 63; ++bit) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
        for (int bit = 48; bit >= 0; bit -= 8) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
    }

    if (stones & (1ULL << 7)) {
        kakutei |= (1ULL << 7);
        for (int bit = 6; bit >= 0; --bit) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
        for (int bit = 15; bit <= 63; bit += 8) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
    }

    if (stones & 1ULL) {
        kakutei |= 1ULL;
        for (int bit = 1; bit <= 7; ++bit) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
        for (int bit = 8; bit <= 56; bit += 8) {
            if (stones & (1ULL << bit)) kakutei |= (1ULL << bit);
            else break;
        }
    }

    return __builtin_popcountll(kakutei);
}


// ============================================================
// 中盤評価関数
// ============================================================

inline int evaluate(const Board& b) {

    int score = 0;


    // --------------------------------------------------------
    // 合法手
    // --------------------------------------------------------

    uint64_t legal_board =
        b.get_legal();


    uint64_t legal_board_opponent =
        calc_legal(
            b.opponent,
            b.player
        );


    // --------------------------------------------------------
    // 開放度
    // --------------------------------------------------------

    uint64_t kaihou_board =
        calc_kaihou(
            b.player,
            b.player,
            b.opponent
        );


    uint64_t kaihou_board_opponent =
        calc_kaihou(
            b.opponent,
            b.player,
            b.opponent
        );
    


    // --------------------------------------------------------
    // 評価
    // --------------------------------------------------------

    for (int i = 0; i < HW2; ++i) {

        int bit_pos = HW2_M1 - i;


        score +=  cell_weight[i]  *  (  int(  (b.player >> bit_pos)  & 1ULL )  -  int(   (b.opponent >> bit_pos)  & 1ULL   ) )+
            C_LEGAL   *   ( int(     (legal_board >> bit_pos)     & 1ULL ) - int(   (legal_board_opponent >> bit_pos)   & 1ULL) )  +
              C_KAIHOU  *  (      int(          (kaihou_board >> bit_pos          & 1ULL    ))   -   int(   (kaihou_board_opponent >> bit_pos)   & 1ULL)   )+
              C_KAKUTEI * (calc_kakutei(b.player) - calc_kakutei(b.opponent))
              ;
    }
    return score;
}


// ============================================================
// 終盤評価関数
// ============================================================

inline int evaluate_endgame(const Board& b) { 

    int score = 0;

    for (int i = 0; i < HW2; i++) {
        score += int((b.player >> i) & 1ULL)
               - int((b.opponent >> i) & 1ULL);
    }

    if (score > SC_W)
        score = SC_W;

    else if (score < -SC_W)
        score = -SC_W;

    return score;
};