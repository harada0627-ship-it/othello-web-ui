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

    // ========================================================
    // 手番・探索深さを受け取る
    // ========================================================

    if (!(std::cin >> player_color >> depth)) {
        std::cout << -1 << std::endl;
        return 0;
    }


    // ========================================================
    // 盤面を受け取る
    //
    // cells:
    // 0 = BLACK
    // 1 = WHITE
    // 2 = VACANT
    //
    // cells[0]  = A1
    // cells[63] = H8
    // ========================================================

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


    // ========================================================
    // 手番チェック
    // ========================================================

    if (
        player_color != BLACK &&
        player_color != WHITE
    ) {
        std::cout << -1 << std::endl;
        return 0;
    }


    if (depth <= 0) {
        std::cout << -1 << std::endl;
        return 0;
    }


    // ========================================================
    // Flip用前計算テーブル初期化
    // ========================================================

    move_init();


    // ========================================================
    // Board生成
    //
    // 現在のBoardでは
    //
    // player   = 現在手番側
    // opponent = 相手側
    //
    // なので黒固定・白固定ではない。
    // ========================================================

    Board b{};

    b.player = 0ULL;
    b.opponent = 0ULL;


    int occupied = 0;


    for (int i = 0; i < HW2; ++i) {

        if (cells[i] == VACANT) {
            continue;
        }


        ++occupied;


        // Python:
        //
        // A1 = index 0
        // H8 = index 63
        //
        // Bitboard:
        //
        // A1 = bit63
        // H8 = bit0
        //
        uint64_t bit =
            1ULL << (HW2_M1 - i);


        // 現在手番側
        if (cells[i] == player_color) {

            b.player |= bit;
        }

        // 相手側
        else {

            b.opponent |= bit;
        }
    }


    // ========================================================
    // Board補助情報
    // ========================================================

    if (occupied >= 4) {
        b.n =
            static_cast<uint_fast8_t>(
                occupied - 4
            );
    }
    else {
        b.n = 0;
    }

    b.parity = 0;

    b.value = 0;
    b.place = -1;


    // ========================================================
    // 合法手確認
    // ========================================================

    uint64_t legal =
        b.get_legal();


    if (legal == 0) {

        // パス
        std::cout << -1 << std::endl;
        return 0;
    }


    // ========================================================
    // AI探索
    //
    // search() の戻り値:
    //
    // A1 = 0
    // B1 = 1
    // ...
    // H8 = 63
    //
    // Pythonのcells[]と同じ番号体系
    // ========================================================

    int move =
        search(
            b,
            depth
        );


    // ========================================================
    // 戻り値チェック
    // ========================================================

    if (
        move < 0 ||
        move >= HW2
    ) {
        std::cout << -1 << std::endl;
        return 0;
    }


    // 本当に合法手かチェック
    uint64_t move_bit =
        1ULL << (HW2_M1 - move);


    if ((legal & move_bit) == 0) {

        std::cout << -1 << std::endl;
        return 0;
    }


    // ========================================================
    // Pythonへ着手位置を返す
    // ========================================================

    std::cout
        << move
        << std::endl;


    return 0;
}
