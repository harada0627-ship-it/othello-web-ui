#pragma once 

#include "board.hpp"     
#include  <iostream>   

#define sc_w 64
#define p31 3
#define p32 9
#define p33 27
#define p34 81
#define p35 243

int mobility_arr[2][n_line];
int surround_arr[2][n_line];
inline int line_score_1[n_line][hw] = {}; 
inline int line_score_2[n_line] = {}; 
inline int cell_weight[hw2] = {
     2714,  147,   69,  -18,  -18,   69,  147, 2714,
      147, -577, -186, -153, -153, -186, -577,  147,
       69, -186, -379, -122, -122, -379, -186,   69,
      -18, -153, -122, -169, -169, -122, -153,  -18,
      -18, -153, -122, -169, -169, -122, -153,  -18,
       69, -186, -379, -122, -122, -379, -186,   69,
      147, -577, -186, -153, -153, -186, -577,  147,
     2714,  147,   69,  -18,  -18,   69,  147, 2714
};

// 係数
constexpr int W_MOBILITY = 30;
constexpr int W_SURROUND = -10;


// 合法手数・開放度の前計算
inline void init_mobility_surround() {
    int idx, place, b, w;

    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, black);
        w = create_one_color(idx, white);

        mobility_arr[black][idx] = 0;
        mobility_arr[white][idx] = 0;

        surround_arr[black][idx] = 0;
        surround_arr[white][idx] = 0;

        // 開放度
        for (place = 0; place < hw; ++place) {
            if (place > 0) {
                bool left_empty =
                    ((b >> (place - 1)) & 1) == 0 &&
                    ((w >> (place - 1)) & 1) == 0;

                if (left_empty) {
                    if ((b >> place) & 1) {
                        ++surround_arr[black][idx];
                    }
                    else if ((w >> place) & 1) {
                        ++surround_arr[white][idx];
                    }
                }
            }
            if (place < hw - 1) {
                bool right_empty =
                    ((b >> (place + 1)) & 1) == 0 &&
                    ((w >> (place + 1)) & 1) == 0;

                if (right_empty) {
                    if ((b >> place) & 1) {
                        ++surround_arr[black][idx];
                    }
                    else if ((w >> place) & 1) {
                        ++surround_arr[white][idx];
                    }
                }
            }
        }

        // 合法手数
        
        for (place = 0; place < hw; ++place) {
            if (legal_arr[black][idx][place]) {
                ++mobility_arr[black][idx];
            }

            if (legal_arr[white][idx][place]) {
                ++mobility_arr[white][idx];
            }
        }
    }
}

// 短い斜めライン補正

inline int sfill5(int idx) {
    return pop_digit[idx][2] != 2 ? idx - p35 + 1 : idx;
}

inline int sfill4(int idx) {
    return pop_digit[idx][3] != 2 ? idx - p34 + 1 : idx;
}

inline int sfill3(int idx) {
    return pop_digit[idx][4] != 2 ? idx - p33 + 1 : idx;
}

inline int sfill2(int idx) {
    return pop_digit[idx][5] != 2 ? idx - p32 + 1 : idx;
}

inline int sfill1(int idx) {
    return pop_digit[idx][6] != 2 ? idx - p31 + 1 : idx;
}

// 近似合法手数
inline int calc_mobility(const board& b) {
    int p = b.player;

    int res =
        mobility_arr[p][b.board_idx[0]] +
        mobility_arr[p][b.board_idx[1]] +
        mobility_arr[p][b.board_idx[2]] +
        mobility_arr[p][b.board_idx[3]] +
        mobility_arr[p][b.board_idx[4]] +
        mobility_arr[p][b.board_idx[5]] +
        mobility_arr[p][b.board_idx[6]] +
        mobility_arr[p][b.board_idx[7]] +

        mobility_arr[p][b.board_idx[8]] +
        mobility_arr[p][b.board_idx[9]] +
        mobility_arr[p][b.board_idx[10]] +
        mobility_arr[p][b.board_idx[11]] +
        mobility_arr[p][b.board_idx[12]] +
        mobility_arr[p][b.board_idx[13]] +
        mobility_arr[p][b.board_idx[14]] +
        mobility_arr[p][b.board_idx[15]] +

        mobility_arr[p][b.board_idx[16] - p35 + 1] +
        mobility_arr[p][b.board_idx[26] - p35 + 1] +
        mobility_arr[p][b.board_idx[27] - p35 + 1] +
        mobility_arr[p][b.board_idx[37] - p35 + 1] +

        mobility_arr[p][b.board_idx[17] - p34 + 1] +
        mobility_arr[p][b.board_idx[25] - p34 + 1] +
        mobility_arr[p][b.board_idx[28] - p34 + 1] +
        mobility_arr[p][b.board_idx[36] - p34 + 1] +

        mobility_arr[p][b.board_idx[18] - p33 + 1] +
        mobility_arr[p][b.board_idx[24] - p33 + 1] +
        mobility_arr[p][b.board_idx[29] - p33 + 1] +
        mobility_arr[p][b.board_idx[35] - p33 + 1] +

        mobility_arr[p][b.board_idx[19] - p32 + 1] +
        mobility_arr[p][b.board_idx[23] - p32 + 1] +
        mobility_arr[p][b.board_idx[30] - p32 + 1] +
        mobility_arr[p][b.board_idx[34] - p32 + 1] +

        mobility_arr[p][b.board_idx[20] - p31 + 1] +
        mobility_arr[p][b.board_idx[22] - p31 + 1] +
        mobility_arr[p][b.board_idx[31] - p31 + 1] +
        mobility_arr[p][b.board_idx[33] - p31 + 1] +

        mobility_arr[p][b.board_idx[21]] +
        mobility_arr[p][b.board_idx[32]];

    return (b.player == black ? res : -res);
}

// 近似開放度・囲み具合
inline int calc_surround(const board& b, int p) {
    return
        surround_arr[p][b.board_idx[0]] +
        surround_arr[p][b.board_idx[1]] +
        surround_arr[p][b.board_idx[2]] +
        surround_arr[p][b.board_idx[3]] +
        surround_arr[p][b.board_idx[4]] +
        surround_arr[p][b.board_idx[5]] +
        surround_arr[p][b.board_idx[6]] +
        surround_arr[p][b.board_idx[7]] +

        surround_arr[p][b.board_idx[8]] +
        surround_arr[p][b.board_idx[9]] +
        surround_arr[p][b.board_idx[10]] +
        surround_arr[p][b.board_idx[11]] +
        surround_arr[p][b.board_idx[12]] +
        surround_arr[p][b.board_idx[13]] +
        surround_arr[p][b.board_idx[14]] +
        surround_arr[p][b.board_idx[15]] +

        surround_arr[p][sfill5(b.board_idx[16])] +
        surround_arr[p][sfill5(b.board_idx[26])] +
        surround_arr[p][sfill5(b.board_idx[27])] +
        surround_arr[p][sfill5(b.board_idx[37])] +

        surround_arr[p][sfill4(b.board_idx[17])] +
        surround_arr[p][sfill4(b.board_idx[25])] +
        surround_arr[p][sfill4(b.board_idx[28])] +
        surround_arr[p][sfill4(b.board_idx[36])] +

        surround_arr[p][sfill3(b.board_idx[18])] +
        surround_arr[p][sfill3(b.board_idx[24])] +
        surround_arr[p][sfill3(b.board_idx[29])] +
        surround_arr[p][sfill3(b.board_idx[35])] +

        surround_arr[p][sfill2(b.board_idx[19])] +
        surround_arr[p][sfill2(b.board_idx[23])] +
        surround_arr[p][sfill2(b.board_idx[30])] +
        surround_arr[p][sfill2(b.board_idx[34])] +

        surround_arr[p][sfill1(b.board_idx[20])] +
        surround_arr[p][sfill1(b.board_idx[22])] +
        surround_arr[p][sfill1(b.board_idx[31])] +
        surround_arr[p][sfill1(b.board_idx[33])] +

        surround_arr[p][b.board_idx[21]] +
        surround_arr[p][b.board_idx[32]];
}


//------------------------------実行部分----------------------------------------


inline void evaluate_init() {

    init_mobility_surround();
    for (int line = 0; line < n_line; line++) {
           int b=create_one_color(line, black);
           int w=create_one_color(line, white);
        for (int i = 0; i < hw; i++) {

        line_score_2[line]+= (1&(b>>(8-1-i)))-(1&(w>>(8-1-i)));
            for(int j=0;j<hw;j++)
        line_score_1[line][i]+=(1&(b>>(8-1-j)))*cell_weight[j+8*i]-(1&(w>>(8-1-j)))*cell_weight[j+8*i];
                }
            }        
}

inline int evaluate(const board& b) { 

    int score = 0;
    for (int i = 0; i < 8; i++) {
        int line = b.board_idx[i];
        score += line_score_1[line][i];
    }
    int mobility = calc_mobility(b);
    score += W_MOBILITY * mobility;
    int black_sur = calc_surround(b, black);
    int white_sur = calc_surround(b, white);
    int surround_score = black_sur - white_sur;

     score += W_SURROUND * surround_score;

    if (b.player == white) score = -score;
 int value = (score >= 0) ? (score + 128) / 256 : (score - 128) / 256;

    if (value > sc_w) value = sc_w;
    else if (value < -sc_w) value = -sc_w;

    return value;
}


inline int evaluate_endgame(const board& b) { 

    int score = 0;
    for (int i = 0; i < 8; i++) {
        int line = b.board_idx[i];
        score += line_score_2[line];
    }
    if (b.player == white) score = -score;
    if (score > sc_w) score =sc_w;
    else if (score < -sc_w) score = -sc_w;
    return score;
}

