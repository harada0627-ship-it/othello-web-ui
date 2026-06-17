#pragma once
#include <iostream>
#include <cstdint>

using namespace std;
#define hw  8
#define hw2  64
#define black 0
#define white 1
#define vacant 2


class board{
public:
uint64_t player_board;
uint64_t opponent_board;
int player;
int value;
int place;

public:
//ハッシュテーブル用のoperator
 bool operator==(const board& another) const {
            if (this->player_board != another.player_board)
                return false;
            if(this->opponent_board != another.opponent_board)
            return false;
        return true;
        }

bool operator!=(const board& another) const {
         return !(this->operator==(another));
        }

//ハッシュテーブル用のハッシュ関数
struct hash {
    typedef size_t result_type;

    size_t operator()(const board& b) const noexcept {
        uint64_t h = b.player_board * 0x9E3779B185EBCA87ULL;
        h ^= b.opponent_board * 0xC2B2AE3D27D4EB4FULL;
        h ^= h >> 32;

        return static_cast<size_t>(h);
    }
};
//vecoer用のoperator
 bool operator<(const board& another) const {
          return this-> value > another.value;
        }




inline int kakutei_opponent()const{
    const uint64_t mask_right = opponent_board & 0x8000000000000080ULL; //右
    const uint64_t mask_left = opponent_board &  0x0100000000000001ULL; //左
    const uint64_t mask_above = opponent_board & 0x0000000000000081ULL; //上
    const uint64_t mask_under = opponent_board & 0x8100000000000000ULL; //下
    uint64_t res_board = 0;
    uint64_t res = 0;
    
    res_board |= (mask_right >> 1 ) & opponent_board;
    res_board |= (res_board >> 1 ) & opponent_board;  
    res_board |= (res_board >> 1 ) & opponent_board;
    res_board |= (res_board >> 1 ) & opponent_board;
    res_board |= (res_board >> 1 ) & opponent_board;
    res_board |= (res_board >> 1 ) & opponent_board;
    res_board |= (res_board >> 1 ) & opponent_board;
    res |= res_board; 
    res_board = 0;

    res_board |= (mask_left << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res_board |= (res_board << 1 ) & opponent_board; 
    res |= res_board; 
    res_board = 0;

    res_board |= (mask_above << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res_board |= (res_board << 8 ) & opponent_board; 
    res |= res_board; 
    res_board = 0;
   
   res_board |= ( mask_under >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
   res_board |= (res_board >> 8 ) & opponent_board;
    res |= res_board; 
    res_board = 0;

    int count = 0;
    for(int i = 0; i< hw2;i++){
        if(res >>i & 1ULL)
        count++;
    }
    return count;
}

inline int kakutei_player() const{

    const uint64_t mask_right = player_board & 0x8000000000000080ULL; // 右方向用
    const uint64_t mask_left  = player_board & 0x0100000000000080ULL; // 左方向用
    const uint64_t mask_above = player_board & 0x0000000000000081ULL; // 上方向用
    const uint64_t mask_under = player_board & 0x8100000000000000ULL; // 下方向用

    uint64_t res_board = 0;
    uint64_t res = 0;

    // 角・端の起点も数える
    res |= mask_right;
    res |= mask_left;
    res |= mask_above;
    res |= mask_under;

    res_board |= (mask_right >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res_board |= (res_board >> 1) & player_board;
    res |= res_board;
    res_board = 0;

    res_board |= (mask_left << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res_board |= (res_board << 1) & player_board;
    res |= res_board;
    res_board = 0;

    res_board |= (mask_above << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res_board |= (res_board << 8) & player_board;
    res |= res_board;
    res_board = 0;

    res_board |= (mask_under >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res_board |= (res_board >> 8) & player_board;
    res |= res_board;

    int count = 0;
    for (int i = 0; i < hw2; i++) {
        if ((res >> i) & 1ULL) {
            count++;
        }
    }

    return count;
}


inline uint64_t legal()const{

    const uint64_t mask_side = opponent_board & 0x7e7e7e7e7e7e7e7eULL;
    const uint64_t mask_ver = opponent_board & 0x00FFFFFFFFFFFF00ULL;
    const uint64_t mask_all = opponent_board& 0x007e7e7e7e7e7e00ULL;
    const uint64_t vacant_board = ~(player_board | opponent_board);
    uint64_t legal_board;
    uint64_t res = 0;
    //左
    res = (player_board<<1) & mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    legal_board= (res<<1 )& vacant_board;
    //右
    res = (player_board>>1) & mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    legal_board |= (res >> 1 )& vacant_board;
   //上
    res = (player_board<<8) & mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    legal_board |= (res<<8)& vacant_board;
    //下
    res = (player_board>>8) & mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    legal_board |= (res >> 8 )& vacant_board;
    //右下がり斜め上
    res = (player_board<<9) & mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    legal_board |= (res<<9)& vacant_board;
    //右下がり斜め下
    res = (player_board>>9) & mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    legal_board |= (res >> 9 )& vacant_board;
    //右下がり斜め上
    res = (player_board<<7) & mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    legal_board |= (res<<7)& vacant_board;
    //右下がり斜め下
    res = (player_board>>7) & mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    legal_board |= (res >> 7 )& vacant_board;
    return legal_board;  
    }



    inline uint64_t legal_opponent()const{

    const uint64_t mask_side =player_board & 0x7e7e7e7e7e7e7e7eULL;
    const uint64_t mask_ver = player_board & 0x00FFFFFFFFFFFF00ULL;
    const uint64_t mask_all = player_board& 0x007e7e7e7e7e7e00ULL;
    const uint64_t vacant_board = ~(player_board | opponent_board);
    uint64_t legal_board;
    uint64_t res = 0;
    //左
    res = (opponent_board<<1) & mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    res |= (res<<1)& mask_side;
    legal_board= (res<<1 )& vacant_board;
    //右
    res = (opponent_board>>1) & mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    res |= (res>>1)& mask_side;
    legal_board |= (res >> 1 )& vacant_board;
   //上
    res = (opponent_board<<8) & mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    res |= (res<<8)& mask_ver;
    legal_board |= (res<<8)& vacant_board;
    //下
    res = (opponent_board>>8) & mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    res |= (res>>8)& mask_ver;
    legal_board |= (res >> 8 )& vacant_board;
    //右下がり斜め上
    res = (opponent_board<<9) & mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    res |= (res<<9)& mask_all;
    legal_board |= (res<<9)& vacant_board;
    //右下がり斜め下
    res = (opponent_board>>9) & mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    res |= (res>>9)& mask_all;
    legal_board |= (res >> 9 )& vacant_board;
    //右下がり斜め上
    res = (opponent_board<<7) & mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    res |= (res<<7)& mask_all;
    legal_board |= (res<<7)& vacant_board;
    //右下がり斜め下
    res = (opponent_board>>7) & mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    res |= (res>>7)& mask_all;
    legal_board |= (res >> 7 )& vacant_board;
    return legal_board;  
    }


    inline uint64_t kaihou() const{
        uint64_t kaihou_board = 0;
        uint64_t vacant_board = ~(player_board | opponent_board);
        for(int i = 0;i < hw; i++){
           uint64_t mask =  make_mask(player_board,i);
           kaihou_board |= mask & vacant_board;
        }
        return kaihou_board;
    }

    inline uint64_t kaihou_opponent()const{
        uint64_t kaihou_board = 0;
        uint64_t vacant_board = ~(player_board | opponent_board);
        for(int i = 0;i < hw; i++){
           uint64_t mask =  make_mask(opponent_board,i);
           kaihou_board |= mask & vacant_board;
        }
        return kaihou_board;
    }

    inline board move(const uint64_t p)const{
        board res;
        uint64_t rev = 0;
        res.player_board = this->opponent_board;
        res.opponent_board = this->player_board;

     for (int i = 0; i < 8; i++) {
            uint64_t rev_ = 0;
            uint64_t mask = make_mask(p, i);

            while (mask != 0 && mask & opponent_board) {
                rev_ |= mask;
                mask = make_mask(mask, i);
            }

            if ((mask & player_board) != 0) {
                rev |= rev_;
            }
        }

        res.player_board  ^=rev;
        res.opponent_board ^= rev | p;
        res.player = 1 - this->player;
        return res;

    }


    //[]→bitboard
    inline void trans_bitboard( const int arr[] ){
        this->player_board = 0;
        this->opponent_board = 0;
        for(int i=0;i<64;i++){
          if(arr[i] == player)
            player_board |= 1ULL<<(64-i-1);
         if(arr[i] == 1-player)
            opponent_board |= 1ULL<<(64-i-1);
        }
    };

private:

inline uint64_t make_mask(uint64_t board,int k) const {
        switch (k) {
        case 0: // 上
            return (board & 0x00ffffffffffffffULL) << 8;
        case 1: // 右上
            return (board & 0x00fefefefefefefeULL) << 7;
        case 2: // 右
            return (board & 0xfefefefefefefefeULL) >> 1;
        case 3: // 右下
            return (board & 0xfefefefefefefe00ULL) >> 9;
        case 4: // 下
            return (board & 0xffffffffffffff00ULL) >> 8;
        case 5: // 左下
            return (board & 0x7f7f7f7f7f7f7f00ULL) >> 7;
        case 6: // 左
            return (board  & 0x7f7f7f7f7f7f7f7fULL) << 1;
        case 7: // 左上
            return (board & 0x007f7f7f7f7f7f7fULL) << 9;
        default:
            return 0ULL;
        }
    };

};



