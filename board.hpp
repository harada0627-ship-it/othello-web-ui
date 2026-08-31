#pragma once

#include <iostream>
#include <cstdint>
#include <utility>

#include "common.hpp"
#include "mobility.hpp"
#include "flip.hpp"

using namespace std;


class Board {
public:
    uint64_t player;
    uint64_t opponent;

    uint_fast8_t n;
    uint_fast8_t parity;

    // move ordering 用
    int value;
    int place;


public:

    // ========================================================
    // TT用 operator==
    // ========================================================

    bool operator==(const Board& another) const {

        if (player != another.player)
            return false;

        if (opponent != another.opponent)
            return false;

        return true;
    }


    // ========================================================
    // TT用 operator!=
    // ========================================================

    bool operator!=(const Board& another) const {
        return !(*this == another);
    }


    // ========================================================
    // TT用 hash
    // ========================================================

    struct hash {

        using result_type = size_t;

        size_t operator()(const Board& b) const noexcept {

            uint64_t h =
                b.player
                * 0x9E3779B185EBCA87ULL;

            h ^=
                b.opponent
                * 0xC2B2AE3D27D4EB4FULL;

            h ^= h >> 32;

            return static_cast<size_t>(h);
        }
    };


    // ========================================================
    // move ordering 用 operator<
    //
    // value が大きい盤面を先頭にする
    // ========================================================

    bool operator<(const Board& another) const {
        return value > another.value;
    }


    // ========================================================
    // 合法手生成
    // ========================================================

    inline uint64_t get_legal() const {
        return calc_legal(player, opponent);
    }


    // ========================================================
    // 着手
    // ========================================================

    inline Board move(const Flip *flip) const {

        Board res = *this;

        res.player ^= flip->flip;
        res.opponent ^= flip->flip;
        res.player ^= 1ULL << flip->pos;

        swap(
            res.player,
            res.opponent
        );

        ++res.n;

        return res;
    }
};