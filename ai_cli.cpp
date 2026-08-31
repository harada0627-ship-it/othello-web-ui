// ai_cli.cpp
// Web UI / Python 側から標準入力で盤面を受け取り、
// 現在の bitboard 版 Othello AI の着手を標準出力へ返す。
//
// 入力形式:
//   player depth
//   cell0 cell1 ... cell63
//
// cell:
//   BLACK  = 黒
//   WHITE  = 白
//   VACANT = 空きマス
//
// cells[0]  = A1
// cells[63] = H8
//
// 出力:
//   0〜63 = AI の着手
//   -1    = パス / 入力エラー / 探索エラー

#include <iostream>
#include <cstdint>

#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "ai.hpp"

int main()
{
    int player_color;
    int depth;

    // 手番・探索深さを受け取る
    if (!(std::cin >> player_color >> depth)) {
        std::cout << -1 << std::endl;
        return 0;
    }

    // 手番チェック
    if (player_color != BLACK && player_color != WHITE) {
        std::cout << -1 << std::endl;
        return 0;
    }

    // 探索深さチェック
    if (depth <= 0) {
        std::cout << -1 << std::endl;
        return 0;
    }

    // 盤面を受け取る
    int cells[HW2];

    for (int i = 0; i < HW2; ++i) {
        if (!(std::cin >> cells[i])) {
            std::cout << -1 << std::endl;
            return 0;
        }

        if (
            cells[i] != BLACK &&
            cells[i] != WHITE &&
            cells[i] != VACANT
        ) {
            std::cout << -1 << std::endl;
            return 0;
        }
    }

    // Flip 用前計算テーブル初期化
    move_init();

    // Board 生成
    // Board.player   = 現在手番側
    // Board.opponent = 相手側
    Board b{};

    b.player = 0ULL;
    b.opponent = 0ULL;

    int occupied = 0;

    for (int i = 0; i < HW2; ++i) {
        if (cells[i] == VACANT) {
            continue;
        }

        ++occupied;

        // Python / Web UI 側:
        //   A1 = index 0, H8 = index 63
        // Bitboard 側:
        //   A1 = bit63, H8 = bit0
        const uint64_t bit = 1ULL << (HW2_M1 - i);

        if (cells[i] == player_color) {
            b.player |= bit;
        }
        else {
            b.opponent |= bit;
        }
    }

    // Board 補助情報
    if (occupied >= 4) {
        b.n = static_cast<uint_fast8_t>(occupied - 4);
    }
    else {
        b.n = 0;
    }

    b.parity = 0;
    b.value = 0;
    b.place = -1;

    // 合法手確認
    const uint64_t legal = b.get_legal();

    if (legal == 0) {
        // パス
        std::cout << -1 << std::endl;
        return 0;
    }

    // AI 探索
    // search() の戻り値は cells[] と同じ 0〜63 の番号体系を想定
    const int move = search(b, depth);

    // 戻り値チェック
    if (move < 0 || move >= HW2) {
        std::cout << -1 << std::endl;
        return 0;
    }

    // search() が返した手が本当に合法か確認
    const uint64_t move_bit = 1ULL << (HW2_M1 - move);

    if ((legal & move_bit) == 0) {
        std::cout << -1 << std::endl;
        return 0;
    }

    // Python / Web UI へ着手位置を返す
    std::cout << move << std::endl;

    return 0;
}
