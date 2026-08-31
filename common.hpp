#pragma once

#include <cstdint>

// 盤面サイズ
constexpr int HW      = 8;
constexpr int HW_M1   = 7;
constexpr int HW_P1   = 9;

constexpr int HW2     = 64;
constexpr int HW2_M1  = 63;
constexpr int HW2_P1  = 65;

// 8bitで表現可能なパターン数
constexpr int N_8BIT = 256;

// 斜めライン用
constexpr int N_DIAG_LINE = 11;

// 石の種類
constexpr int BLACK  = 0;
constexpr int WHITE  = 1;
constexpr int VACANT = 2;

// 探索用
constexpr int INF = 100000000;