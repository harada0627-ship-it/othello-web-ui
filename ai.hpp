#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"

using namespace std;


// ============================================================
// 探索用定数
// ============================================================

constexpr int BONUS = 2000;


// ============================================================
// 置換表
// ============================================================

unordered_map<Board, int, Board::hash> transpose_table;
unordered_map<Board, int, Board::hash> former_transpose_table;


// ============================================================
// 子局面を生成
//
// place : bit位置 0～63
// ============================================================

inline Board make_child(
    const Board& b,
    uint_fast8_t place
) {
    Flip flip;

    flip.calc_flip(
        b.player,
        b.opponent,
        place
    );

    return b.move(&flip);
}


// ============================================================
// 中盤 move ordering
// ============================================================

inline int moveordering_value_mid(const Board& b)
{
    auto it = former_transpose_table.find(b);

    if (it != former_transpose_table.end()) {
        return it->second + BONUS;
    }

    return -evaluate(b);
}


// ============================================================
// 終盤 move ordering
//
// 相手の合法手数が少ない局面を優先
// ============================================================

inline int moveordering_value_endgame(Board b)
{
    uint64_t legal_board = b.get_legal();

    return -(__builtin_popcountll(legal_board));
}


// ============================================================
// シンプルなαβ探索
// ============================================================

int nega_alpha(
    Board b,
    int depth,
    bool passed,
    int alpha,
    int beta
) {
    if (depth <= 0) {
        return evaluate_endgame(b);
    }

    uint64_t legal_board = b.get_legal();


    // --------------------------------------------------------
    // パス
    // --------------------------------------------------------

    if (legal_board == 0) {

        // 2回連続パス → 終局
        if (passed) {
            return evaluate_endgame(b);
        }

        // player / opponent を入れ替える
        swap(b.player, b.opponent);

        return -nega_alpha(
            b,
            depth,
            true,
            -beta,
            -alpha
        );
    }


    int score = -INF;


    // --------------------------------------------------------
    // 合法手を展開
    // --------------------------------------------------------

    for (int i = 0; i < HW2; ++i) {

        // i=0 → bit63
        // i=63 → bit0
        uint_fast8_t place =
            static_cast<uint_fast8_t>(
                HW2_M1 - i
            );

        uint64_t p =
            1ULL << place;


        if (legal_board & p) {

            Board child =
                make_child(
                    b,
                    place
                );


            score = max(
                score,
                -nega_alpha(
                    child,
                    depth - 1,
                    false,
                    -beta,
                    -alpha
                )
            );


            alpha =
                max(
                    alpha,
                    score
                );


            if (alpha >= beta) {
                return alpha;
            }
        }
    }


    return score;
}


// ============================================================
// 置換表付きαβ探索
// ============================================================

int nega_alpha_tt(
    Board b,
    int depth,
    bool passed,
    int alpha,
    int beta
) {
    if (depth <= 0) {
        return evaluate(b);
    }


    // --------------------------------------------------------
    // TT
    // --------------------------------------------------------

    auto it =
        transpose_table.find(b);

    if (it != transpose_table.end()) {
        return it->second;
    }


    uint64_t legal_board =
        b.get_legal();


    // --------------------------------------------------------
    // パス
    // --------------------------------------------------------

    if (legal_board == 0) {

        if (passed) {
            return evaluate(b);
        }

        swap(
            b.player,
            b.opponent
        );

        return -nega_alpha_tt(
            b,
            depth,
            true,
            -beta,
            -alpha
        );
    }


    // --------------------------------------------------------
    // 子局面生成
    // --------------------------------------------------------

    vector<Board> child_nodes;


    for (int i = 0; i < HW2; ++i) {

        uint_fast8_t place =
            static_cast<uint_fast8_t>(
                HW2_M1 - i
            );

        uint64_t p =
            1ULL << place;


        if (legal_board & p) {

            Board child =
                make_child(
                    b,
                    place
                );


            child.value =
                moveordering_value_mid(
                    child
                );

            child.place = i;


            child_nodes.push_back(
                child
            );
        }
    }


    // --------------------------------------------------------
    // move ordering
    //
    // Board::operator< を使用
    // --------------------------------------------------------

    if (child_nodes.size() >= 2) {

        sort(
            child_nodes.begin(),
            child_nodes.end()
        );
    }


    // --------------------------------------------------------
    // αβ探索
    // --------------------------------------------------------

    int score = -INF;


    for (const Board& nb : child_nodes) {

        score = max(
            score,
            -nega_alpha_tt(
                nb,
                depth - 1,
                false,
                -beta,
                -alpha
            )
        );


        alpha =
            max(
                alpha,
                score
            );


        if (alpha >= beta) {
            return alpha;
        }
    }


    // TTへ登録
    transpose_table[b] =
        score;


    return score;
}


// ============================================================
// 終盤探索
// ============================================================

int endgame_search(
    Board b,
    int depth,
    bool passed,
    int alpha,
    int beta
) {
    if (depth <= 0) {
        return evaluate_endgame(b);
    }


    // --------------------------------------------------------
    // TT
    // --------------------------------------------------------

    auto it =
        transpose_table.find(b);

    if (it != transpose_table.end()) {
        return it->second;
    }


    uint64_t legal_board =
        b.get_legal();


    // --------------------------------------------------------
    // パス
    // --------------------------------------------------------

    if (legal_board == 0) {

        if (passed) {
            return evaluate_endgame(b);
        }

        swap(
            b.player,
            b.opponent
        );

        return -endgame_search(
            b,
            depth,
            true,
            -beta,
            -alpha
        );
    }


    // --------------------------------------------------------
    // 子局面生成
    // --------------------------------------------------------

    vector<Board> child_nodes;


    for (int i = 0; i < HW2; ++i) {

        uint_fast8_t place =
            static_cast<uint_fast8_t>(
                HW2_M1 - i
            );

        uint64_t p =
            1ULL << place;


        if (legal_board & p) {

            Board child =
                make_child(
                    b,
                    place
                );


            child.value =
                moveordering_value_endgame(
                    child
                );

            child.place = i;


            child_nodes.push_back(
                child
            );
        }
    }


    if (child_nodes.size() >= 2) {

        sort(
            child_nodes.begin(),
            child_nodes.end()
        );
    }


    // --------------------------------------------------------
    // αβ探索
    // --------------------------------------------------------

    int score = -INF;


    for (const Board& nb : child_nodes) {

        score = max(
            score,
            -endgame_search(
                nb,
                depth - 1,
                false,
                -beta,
                -alpha
            )
        );


        alpha =
            max(
                alpha,
                score
            );


        if (alpha >= beta) {
            return alpha;
        }
    }


    transpose_table[b] =
        score;


    return score;
}


// ============================================================
// 最善手探索
//
// 戻り値
//   A1 = 0
//   B1 = 1
//   ...
//   H8 = 63
//
// 合法手なしの場合 -1
// ============================================================

int search(
    Board b,
    int depth
) {
    constexpr int ENDGAME_DEPTH = 20;


    uint64_t legal_board =
        b.get_legal();


    // 合法手なし
    if (legal_board == 0) {
        return -1;
    }


    // --------------------------------------------------------
    // 空きマス数
    // --------------------------------------------------------

    uint64_t vacant_board =
        ~(b.player | b.opponent);


    int count_vacant =
        __builtin_popcountll(
            vacant_board
        );


    int alpha = -INF;
    int beta  = INF;

    int best  = -1;
    int score = -INF;


    // ========================================================
    // 中盤探索
    // ========================================================

    if (count_vacant > ENDGAME_DEPTH) {

        vector<Board> child_nodes;


        // ----------------------------------------------------
        // 子局面生成
        // ----------------------------------------------------

        for (int i = 0; i < HW2; ++i) {

            uint_fast8_t place =
                static_cast<uint_fast8_t>(
                    HW2_M1 - i
                );

            uint64_t p =
                1ULL << place;


            if (legal_board & p) {

                Board child =
                    make_child(
                        b,
                        place
                    );


                child.value =
                    moveordering_value_mid(
                        child
                    );

                // search() が返す着手番号
                child.place = i;


                child_nodes.push_back(
                    child
                );
            }
        }


        if (child_nodes.size() >= 2) {

            sort(
                child_nodes.begin(),
                child_nodes.end()
            );
        }


        // ----------------------------------------------------
        // 反復深化
        //
        // 元コードと同じ
        //
        // depth-3
        // depth-2
        // depth-1
        // ----------------------------------------------------

        for (int i = 2; i >= 0; --i) {

            alpha = -INF;
            beta  = INF;

            score = -INF;
            best  = -1;


            for (const Board& nb : child_nodes) {

                score =
                    -nega_alpha_tt(
                        nb,
                        depth - 1 - i,
                        false,
                        -beta,
                        -alpha
                    );


                if (score > alpha) {

                    best =
                        nb.place;

                    alpha =
                        score;
                }
            }


            // 今回の探索結果を次の反復深化の
            // move ordering に利用
            transpose_table.swap(
                former_transpose_table
            );

            transpose_table.clear();
        }


        return best;
    }


    // ========================================================
    // 終盤完全読み
    // ========================================================

    else {

        vector<Board> child_nodes;


        for (int i = 0; i < HW2; ++i) {

            uint_fast8_t place =
                static_cast<uint_fast8_t>(
                    HW2_M1 - i
                );

            uint64_t p =
                1ULL << place;


            if (legal_board & p) {

                Board child =
                    make_child(
                        b,
                        place
                    );


                child.value =
                    moveordering_value_endgame(
                        child
                    );

                child.place = i;


                child_nodes.push_back(
                    child
                );
            }
        }


        if (child_nodes.size() >= 2) {

            sort(
                child_nodes.begin(),
                child_nodes.end()
            );
        }


        for (const Board& nb : child_nodes) {

            score =
                -endgame_search(
                    nb,
                    count_vacant - 1,
                    false,
                    -beta,
                    -alpha
                );


            if (score > alpha) {

                best =
                    nb.place;

                alpha =
                    score;
            }
        }


        return best;
    }
}