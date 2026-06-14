// ai_cli.cpp
// Web UI のNode.jsサーバーから呼び出されるC++ AI実行ファイル。
// ai1.cpp / board.hpp / cell_evaluate.hpp と同じディレクトリに置いてコンパイルしてください。
//
// 入力形式(stdin):
//   player depth
//   cell0 cell1 ... cell63
//
// cell:
//   0 = black
//   1 = white
//   2 = vacant
//
// 出力(stdout):
//   0〜63 = AIの着手
//   -1    = パス/エラー

#include <iostream>
#include "board_bit.hpp"
#include "evaluate.hpp"
#include "ai_bitboard.cpp"

int main() {
    int player;
    int depth;

    if (!(std::cin >> player >> depth)) {
        std::cout << -1 << std::endl;
        return 0;
    }

    int cells[64];
    for (int i = 0; i < 64; ++i) {
        if (!(std::cin >> cells[i])) {
            std::cout << -1 << std::endl;
            return 0;
        }
    }

    if (player != black && player != white) {
        std::cout << -1 << std::endl;
        return 0;
    }


    board b;
    b.player =player;
    b.trans_bitboard(cells);
    

    bool has_legal = false;
    for (int i = 0; i < hw2; ++i) {
        if (!( b.legal() == 0 )) {
            has_legal = true;
            break;
        }
    }

    if (!has_legal) {
        std::cout << -1 << std::endl;
        return 0;
    }

    int move = search(b, depth);

    if (move < 0 || move >= hw2) {
        std::cout << -1 << std::endl;
        return 0;
    }

    std::cout << move << std::endl;
    return 0;
}
