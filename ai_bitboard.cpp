#include<iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cstdint>
#define inf 10000000
#define bonus 2000

using namespace std;

unordered_map<board, int, board::hash> transpose_table; 
unordered_map<board, int, board::hash> former_transpose_table; 


inline int moveordering_value_mid(const board& b){
    auto it = former_transpose_table.find(b);
    if(it != former_transpose_table.end())
    return it->second + bonus;
else return -evaluate(b);
};

inline int moveordering_value_endgame(const board& b){
uint64_t legal_board = b.legal();
int count_legal=0;
for(int i = 0;i++;i<hw2){
if(legal_board >> (63-i) & 1ULL)
count_legal ++;
}
 return count_legal;
};

//シンプルなαβ
int nega_alpha(board b,int depth,bool passed,int alpha,int beta){
    if(depth <= 0)
     return evaluate_endgame(b);

     uint64_t legal_board = b.legal();

    if(legal_board == 0){
     if(passed == true){
     return evaluate_endgame(b);
    }
    b.player = 1-b.player;
    uint64_t res2 = b.player_board;
    b.player_board = b.opponent_board;
    b.opponent_board = res2;
    return -nega_alpha(b,depth,true,-beta,-alpha);
    }

    int score = -inf;
    for(int i = 0;i < hw2; i++){
        uint64_t p = 1ULL << (64 - 1 - i);
        if(legal_board & p){
        score = max (-nega_alpha(b.move(p),depth-1,false,-beta,-alpha),score);
        alpha = max(score,alpha);
        if(alpha >= beta)
        return alpha;
        } 
    }
    return score;
}




//置換表付きαβ
int nega_alpha_tt(board b,int depth,bool passed,int alpha,int beta){
    if(depth <= 0)
     return evaluate(b);

     if (transpose_table.find(b) != transpose_table.end())
        return transpose_table[b];

     uint64_t legal_board = b.legal();

    if(legal_board == 0){
     if(passed == true){
     return evaluate(b);
    }
    b.player = 1-b.player;
    uint64_t res2 = b.player_board;
    b.player_board = b.opponent_board;
    b.opponent_board = res2;
    return -nega_alpha_tt(b,depth,true,-beta,-alpha);
    }

    
     //moveoering
    vector<board> child_nodes;
    int canput = 0;
    for (int i = 0; i < hw2; i++) {
        uint64_t p = 1ULL << (63 - i);
        if (legal_board>> (63 -i) & 1ULL ) {
            child_nodes.push_back(b.move(p));
            child_nodes[canput].value = moveordering_value_mid(child_nodes[canput]);
            child_nodes[canput].place = i;
            canput++;
        }
    }
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());


    int score = -inf;
    for(const board nb:child_nodes){
        score = max (-nega_alpha_tt(nb,depth-1,false,-beta,-alpha),score);
        alpha = max(score,alpha);
        if(alpha >= beta)
        return alpha;
    }
    transpose_table[b] = score;
    return score;
}



//終盤探索
int endgame_search(board b,int depth,bool passed,int alpha,int beta){
    if(depth <= 0)
     return evaluate_endgame(b);

     if (transpose_table.find(b) != transpose_table.end())
        return transpose_table[b];

     uint64_t legal_board = b.legal();

    if(legal_board == 0){
     if(passed == true){
     return evaluate_endgame(b);
    }
    b.player = 1-b.player;
    uint64_t res2 = b.player_board;
    b.player_board = b.opponent_board;
    b.opponent_board = res2;
    return -endgame_search(b,depth,true,-beta,-alpha);
    }

    vector<board>child_nodes;
    int canput = 0;
    for (int i = 0; i < hw2; i++) {
        uint64_t p = 1ULL << (63 - i);
        if (legal_board>> (63 -i) & 1ULL ) {
            child_nodes.push_back(b.move(p));
            child_nodes[canput].value = moveordering_value_endgame(child_nodes[canput]);
            canput++;
        }
    }
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());


    int score = -inf;
    for(board nb : child_nodes){
        score = max (-endgame_search(nb,depth-1,false,-beta,-alpha),score);
        alpha = max(score,alpha);
        if(alpha >= beta)
        return alpha;
        } 
    transpose_table[b] = score;
    return score;
}

//search
int search(board b,int depth){
    int count_vacant = 0;
     uint64_t legal_board = b.legal();
     uint64_t vacant_board = ~(b.player_board|b.opponent_board);
     int alpha = -inf;
     int beta = inf;
     int best = -1;
     int score = -inf;
     int canput = 0;
     int endgame_depth = 18;
    
     for(int i = 0; i < hw2; i++){
        if( (vacant_board >>i) & 1ULL)
        count_vacant++;
     }


 if(count_vacant > endgame_depth){
    vector<board> child_nodes;
    canput = 0;

    for (int i = 0; i < hw2; i++) {
        uint64_t p = 1ULL << (63 - i);
        if (legal_board>> (63 -i) & 1ULL) {
            child_nodes.push_back(b.move(p));
            child_nodes[canput].value = moveordering_value_mid(child_nodes[canput]);
            child_nodes[canput].place = i; 
            canput++;
        }
    }
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());

 for(int i = 2;i >= 0;i--){
    alpha = -inf;
    beta = inf;
    score = -inf;
    best = -1;
    for(const board nb:child_nodes){
        score = -nega_alpha_tt(nb,depth - 1 - i,false,-beta,-alpha);
        if(score > alpha){
             best = nb.place;
            alpha = score;
            }
        }
        transpose_table.swap(former_transpose_table);
        transpose_table.clear();
         }
        return best;

    }else{
    vector<board> child_nodes;
    canput = 0;
    for (int i = 0; i < hw2; i++) {
        uint64_t p = 1ULL << (63 - i);
        if (legal_board>> (63 -i) & 1ULL) {
            child_nodes.push_back(b.move(p));
            child_nodes[canput].value = moveordering_value_endgame(child_nodes[canput]);
            child_nodes[canput].place = i;
            canput++;
        }
    }
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());
    
    for(const board nb:child_nodes){
        score = -endgame_search(nb,count_vacant - 1,false,-beta,-alpha);
        if(score > alpha){
             best = nb.place; 
            alpha = score;
            }
        }
    }

    return best;

}