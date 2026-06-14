#pragma once
#include <iostream>
#include "board_bit.hpp"

using namespace std;

#define sc_w 64
#define c_kaihou -2
#define c_legal 7

inline int cell_weight[hw2] = {
     30, -12,  0, -1, -1,  0, -12,  30,
    -12, -15, -3, -3, -3, -3, -15, -12,
      0,  -3,  0, -1, -1,  0,  -3,   0,
     -1,  -3, -1, -1, -1, -1,  -3,  -1,
     -1,  -3, -1, -1, -1, -1,   -3,  -1, 
     0,  -3,  0, -1, -1,  0,   -3,  0,
     -12, -15, -3, -3, -3, -3,  -15, -12,
     30, -12,  0,  -1, -1, 0,  -12, 30,
};


inline int evaluate(const board& b) { 

    int score = 0;
    uint64_t legal_board = b.legal();
    uint64_t legal_board_opponent = b.legal_opponent();
    uint64_t kaihou_board = b.kaihou();
    uint64_t kaihou_board_opponent = b.kaihou_opponent();
    
    for (int i = 0; i < hw2; i++) {
        score += cell_weight[i] *( int(b.player_board >>( 63 - i) & 1ULL) - int( (b.opponent_board >>(63- i) & 1ULL)) ) + 
                c_legal * (int( (legal_board >> (63-i) )& 1ULL) - int( (legal_board_opponent >> (63-i) )& 1ULL)) +
                 c_kaihou * (int( (kaihou_board >> (63-i) )& 1ULL) - int( (kaihou_board_opponent >> (63-i )& 1ULL)) );
    }
    
    int value = score;

    return value;
};

inline int evaluate_endgame(const board& b) { 

    int score = 0;
    for (int i = 0; i < hw2; i++) {
     score += int((b.player_board >> i & 1ULL))  - int((b.opponent_board >> i & 1ULL ));
    }
    if (score > sc_w) score =sc_w;
    else if (score < -sc_w) score = -sc_w;
    return score;
};


