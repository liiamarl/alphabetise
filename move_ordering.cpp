#include "bit_function.hpp"
#include <iostream>
#include "structs.hpp"
#include "chessboard.hpp"
#include <array>
#include <cstdlib>
#include <ctime>
#include <string>
#include "search.hpp"
#include <algorithm>



// move ordering based mostly on SEE, some other parameters are taken into consideration

std::vector<move> chessboard::moves_in_order(bitboard hash_move){
  std::vector<move> moves = get_all_moves();
  if (in_check){
    for (auto it = moves.begin(); it != moves.end(); ++it){
      if ((*it).from_b & hash_move && (*it).to_b & hash_move){ (*it).eval = 1000;}
      else{ (*it).eval = SEE(*it);}
      (*it).depth_reduction = 0;
    }
    std::sort(moves.begin(), moves.end(), sort_moves);
    return moves;
  }

  std::array<bitboard, 12> checks = {
    0, 0, 0,
    S_E(bitboards[black + king]) | S_W(bitboards[black + king]), 0,
    get_knight_attack(bitboards[black + king]), 0,
    get_bishop_attack(bitboards[black + king]), 0,
    get_rook_attack(bitboards[black + king]), 0,0
  };
  checks[white + queen] = checks[white + bishop] | checks[white + rook];

  for (auto it = moves.begin(); it != moves.end(); ++it){
    (*it).depth_reduction = 1;
    if ((*it).from_b & hash_move && (*it).to_b & hash_move){ (*it).eval = 1000;}
    else{
      if ((*it).type == castle){ (*it).eval = 30;}
      else{
        (*it).eval = SEE(*it);
        if ((*it).type == en_passant){ (*it).eval += 30;}
        if ((*it).type == quiet){
          if (phase != endgame){
            if ((*it).piece == white + rook && !(north_line((*it).to_b) & bitboards[white + pawn])){
              //rook to semi open file
              (*it).eval += 15;
              if (!(north_line((*it).to_b) & bitboards[black + pawn])){ (*it).eval += 15;}
            }
          }

          if ((*it).piece == white + pawn && !(north_line((*it).to_b | E((*it).to_b) | W((*it).to_b)) & bitboards[black + pawn])){
            //creates or pushes a passed pawn
            (*it).eval += 20;
            if (phase == early_end || phase == endgame){ (*it).eval += 30;}
          }
          if ((*it).piece == white + pawn && (N_E((*it).to_b) | N_W((*it).to_b)) & (bitboards[black] ^ bitboards[black + pawn])){
            //pawn move with tempo
            (*it).eval += 20;
          }
        }
        if ((*it).to_b & checks[(*it).piece]){ (*it).eval += 50;}
      }
    }
    if ((*it).eval < -350){ (*it).depth_reduction = 2;}
  }
  std::sort(moves.begin(), moves.end(), sort_moves);
  for (auto it = moves.begin(); (it != moves.end() && it != moves.begin() + 4); ++it){
    if ((*it).eval > moves[0].eval - 150){ (*it).depth_reduction = 0;}
  }

  return moves;
}





//moves used in the quiescence search, only captures and promotions with a good SEE are kept
// a maximum of 4 moves are kept

std::vector<move> chessboard::quiescence_moves(){
  std::vector<move> moves = get_all_moves();
  if (in_check){
    for (auto it = moves.begin(); it != moves.end(); ++it){
      (*it).eval = SEE(*it);
      (*it).depth_reduction = 0;
    }
    std::sort(moves.begin(), moves.end(), sort_moves);
    return moves;
  }

  std::array<bitboard, 12> checks = {
    0, 0, 0,
    S_E(bitboards[black + king]) | S_W(bitboards[black + king]), 0,
    get_knight_attack(bitboards[black + king]), 0,
    get_bishop_attack(bitboards[black + king]), 0,
    get_rook_attack(bitboards[black + king]), 0,0
  };
  checks[white + queen] = checks[white + bishop] | checks[white + rook];
  for (auto it = moves.begin(); it != moves.end(); ++it){
    (*it).depth_reduction = 0;
    if ((*it).type == capture || (*it).type == en_passant || (*it).type == promotion || (*it).type == promo_capture){
      (*it).eval = SEE(*it);
      if ((*it).to_b & checks[(*it).piece]){ (*it).eval += 50;}
      if ((*it).eval < 80){
        moves.erase(it);
        --it;
      }
    }
    else{
      if ((*it).to_b & checks[(*it).piece]){(*it).eval = 0;}
      else{
        if (phase == endgame && (*it).piece == white + pawn && !(north_line((*it).to_b | E((*it).to_b) | W((*it).to_b)) & bitboards[black + pawn])){
          (*it).eval = 1;
        }
        else{
          moves.erase(it);
          --it;
        }

      }
    }
  }
  std::sort(moves.begin(), moves.end(), sort_moves);
  if (moves.size() > 4){
    moves.erase(moves.begin() + 4, moves.end());
  }
  return moves;
}



//deep quiescence, with only moves with verry good SEE and 2 of them max

std::vector<move> chessboard::final_moves(){
  std::vector<move> moves, temp;
  moves = stage_1_moves();
  for (auto it = moves.begin(); it != moves.end(); ++it){
    (*it).depth_reduction = 0;
    (*it).eval = SEE(*it);
    if(!(north_line((*it).to_b | E((*it).to_b) | W((*it).to_b)) & bitboards[black + pawn])){ (*it).eval += 60;}
    //pawn takes pawn is only played if a passed pawn takes safely
    if ((*it).eval < 150){
      moves.erase(it);
      --it;
    }
  }
  temp = stage_2_moves();
  for (auto it = temp.begin(); it != temp.end(); ++it){
    (*it).depth_reduction = 0;
    (*it).eval = SEE(*it);
    if ((*it).eval > 149){ moves.push_back(*it);}
  }
  std::sort(moves.begin(), moves.end(), sort_moves);
  if (moves.size() > 2){ moves.erase(moves.begin() + 2, moves.end());}
  return moves;
}
