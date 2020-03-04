#include "chessboard.hpp"
#include <vector>
#include <array>
#include <iostream>
#include "bit_function.hpp"
#include "structs.hpp"
#include <cmath>
#include <algorithm>


//this chess engine uses the direction-wise generation of legal moves (DirGolem) described here https://www.chessprogramming.org/DirGolem


void display_move(move m){
  std::array<std::string, 6> type_of_move = {
    "quiet", "capture", "en passant", "castle", "promotion", "promotion capture"
  };
  std::cout << square_display[m.from] << "  " << square_display[m.to] << " ";
  std::cout << piece_display[m.piece] << "  " << type_of_move[m.type] << std::endl;
}

const std::array<int, 15> switch_pieces = {
  white, black, pawn + white, pawn + black, knight + white, knight + black,
  white + bishop, black + bishop, white + rook, black + rook, white + queen,
  black + queen, white + king, black + king, all};




//the following functions generates an attack table witch is necessary for check and absolute pins detection
//in order to get legal moves and not pseudo legal moves
bitboard chessboard::get_pawn_attack(bitboard const& bit, bool white_){
  if (white_){ return N_W(bit) | N_E(bit);}
 return S_W(bit) | S_E(bit);
}


bitboard chessboard::get_knight_attack(bitboard const& bit){
  return S_S_E(bit) | S_S_W(bit) | S_E_E(bit) | S_W_W(bit) | N_N_E(bit) | N_N_W(bit) | N_W_W(bit) | N_E_E(bit);
}


bitboard chessboard::N_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & ~rank_8) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  temp |= (temp & (~rank_8 & ~(temp & occup))) << 8;
  return temp;
}


bitboard chessboard::S_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & ~rank_1) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  temp |= (temp & (~rank_1 & ~(temp & occup))) >> 8;
  return temp;
}


bitboard chessboard::E_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & ~h_file) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  temp |= (temp & (~h_file & ~(temp & occup))) >> 1;
  return temp;
}


bitboard chessboard::W_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & ~a_file) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  temp |= (temp & (~a_file & ~(temp & occup))) << 1;
  return temp;
}


bitboard chessboard::NE_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & (~h_file & ~rank_8)) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  temp |= (temp & ((~h_file & ~rank_8) & ~(temp & occup))) << 7;
  return temp;
}


bitboard chessboard::NW_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & (~a_file & ~rank_8)) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  temp |= (temp & ((~a_file & ~rank_8) & ~(temp & occup))) << 9;
  return temp;
}


bitboard chessboard::SE_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & (~h_file & ~rank_1)) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  temp |= (temp & ((~h_file & ~rank_1) & ~(temp & occup))) >> 9;
  return temp;
}


bitboard chessboard::SW_attacks(bitboard const& bit, bitboard occup){
  bitboard temp = 0;
  temp |= (bit & (~a_file & ~rank_1)) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  temp |= (temp & ((~a_file & ~rank_1) & ~(temp & occup))) >> 7;
  return temp;
}


bitboard chessboard::get_bishop_attack(bitboard const& bit){
  return SE_attacks(bit, bitboards[all]) | NW_attacks(bit, bitboards[all]) | NE_attacks(bit, bitboards[all]) | SW_attacks(bit, bitboards[all]);
}




bitboard chessboard::get_rook_attack(bitboard const& bit){
  return S_attacks(bit, bitboards[all]) | N_attacks(bit, bitboards[all]) | E_attacks(bit, bitboards[all]) | W_attacks(bit, bitboards[all]);
}


bitboard chessboard::get_queen_attack(bitboard const& bit){
  return get_rook_attack(bit) | get_bishop_attack(bit);
}

bitboard chessboard::get_king_attack(bitboard const& bit){
  return N(bit) | S(bit) | E(bit) | W(bit) | N_E(bit) | N_W(bit) | S_E(bit) | S_W(bit);
}

void chessboard::get_att_table(bool white_){
  attack_table[white_] = 0;
  attack_table[pawn + white_] = get_pawn_attack(bitboards[pawn + white_], white_);
  attack_table[white_] |= attack_table[pawn + white_];
  attack_table[knight + white_] = get_knight_attack(bitboards[knight + white_]);
  attack_table[white_] |= attack_table[knight + white_];
  attack_table[bishop + white_] = get_bishop_attack(bitboards[bishop + white_]);
  attack_table[white_] |= attack_table[bishop + white_];
  attack_table[rook + white_] = get_rook_attack(bitboards[rook + white_]);
  attack_table[white_] |= attack_table[rook + white_];
  attack_table[queen + white_] = get_queen_attack(bitboards[queen + white_]);
  attack_table[white_] |= attack_table[queen + white_];
  attack_table[king + white_] = get_king_attack(bitboards[king + white_]);
  attack_table[white_] |= attack_table[king + white_];
}


//generates a direction classed move table witch is then used for generating a move list
void chessboard::get_dirgolem(){

  bitboard king_att_cross, king_att_dia, ennemy_att, temp_att, king_super_att;

  //checking ennemy attack in order to detect check and pined pieces

  //rooks and queen
  temp_att = W_attacks(bitboards[black + rook] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att = temp_att;
  king_super_att = E_attacks(bitboards[white + king], bitboards[all]);
  king_att_cross = king_super_att;
  hor_block = king_super_att & temp_att;

  temp_att = E_attacks(bitboards[black + rook] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = W_attacks(bitboards[white + king], bitboards[all]);
  king_att_cross |= king_super_att;
  hor_block |= king_super_att & temp_att;

  temp_att = S_attacks(bitboards[black + rook] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = N_attacks(bitboards[white + king], bitboards[all]);
  king_att_cross |= king_super_att;
  ver_block = king_super_att & temp_att;

  temp_att = N_attacks(bitboards[black + rook] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = S_attacks(bitboards[white + king], bitboards[all]);
  king_att_cross |= king_super_att;
  ver_block |= king_super_att & temp_att;


  //bishop and queen

  temp_att = NW_attacks(bitboards[black + bishop] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = SE_attacks(bitboards[white + king], bitboards[all]);
  king_att_dia = king_super_att;
  dia_block = king_super_att & temp_att;

  temp_att = SE_attacks(bitboards[black + bishop] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = NW_attacks(bitboards[white + king], bitboards[all]);
  king_att_dia |= king_super_att;
  dia_block |= king_super_att & temp_att;

  temp_att = SW_attacks(bitboards[black + bishop] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = NE_attacks(bitboards[white + king], bitboards[all]);
  king_att_dia |= king_super_att;
  ant_block = king_super_att & temp_att;

  temp_att = NE_attacks(bitboards[black + bishop] | bitboards[black + queen], bitboards[all] ^ bitboards[white + king]);
  ennemy_att |= temp_att;
  king_super_att = SW_attacks(bitboards[white + king], bitboards[all]);
  king_att_dia |= king_super_att;
  ant_block |= king_super_att & temp_att;


  //non sliding pieces

  ennemy_att |= get_knight_attack(bitboards[black + knight]);
  ennemy_att |= get_pawn_attack(bitboards[black + pawn], false);
  ennemy_att |= get_king_attack(bitboards[black + king]);

  bitboard all_block = dia_block | ant_block | ver_block | hor_block;

  //checking check
  bitboard block_squares = all_block & ~bitboards[all];
  bitboard all_pins = all_block & bitboards[all];
  bitboard check_from = (king_att_cross & (bitboards[black + rook] | bitboards[black + queen]) |
    (king_att_dia & (bitboards[black + bishop] | bitboards[black + queen])) |
    (get_knight_attack(bitboards[white + king]) & bitboards[black + knight]) |
    (get_pawn_attack(bitboards[white + king], true) & bitboards[black + pawn])
  );
  int a = popcount(check_from);
  in_check = false;
  bitboard target_mask;
  if (a == 0){
    target_mask = ~bitboards[white];
  }
  if (a == 1){
    target_mask = ~bitboards[white] & (block_squares | check_from);
    in_check = true;
  }
  if (a > 1){
    target_mask = 0;
    in_check = true;
  }

  //generating the dirgolem


  bitboard temp_sliders = (bitboards[white + rook] | bitboards[white + queen]) & ~(all_pins ^ hor_block);
  dir_golem[dir_W] = W_attacks(temp_sliders, bitboards[all]) & target_mask;
  dir_golem[dir_E] = E_attacks(temp_sliders, bitboards[all]) & target_mask;

  temp_sliders = (bitboards[rook + white] | bitboards[white + queen]) & ~(all_pins ^ ver_block);
  dir_golem[dir_N] = N_attacks(temp_sliders, bitboards[all]) & target_mask;
  dir_golem[dir_S] = S_attacks(temp_sliders, bitboards[all]) & target_mask;

  temp_sliders = (bitboards[white + bishop] | bitboards[white + queen]) & ~(all_pins ^ dia_block);
  dir_golem[dir_NW] = NW_attacks(temp_sliders, bitboards[all]) & target_mask;
  dir_golem[dir_SE] = SE_attacks(temp_sliders, bitboards[all]) & target_mask;

  temp_sliders = (bitboards[white + bishop] | bitboards[white + queen]) & ~(all_pins ^ ant_block);
  dir_golem[dir_SW] = SW_attacks(temp_sliders, bitboards[all]) & target_mask;
  dir_golem[dir_NE] = NE_attacks(temp_sliders, bitboards[all]) & target_mask;

  dir_golem[dir_NNE] = N_N_E(bitboards[white + knight] & ~all_pins) & target_mask;
  dir_golem[dir_NNW] = N_N_W(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_NEE] = N_E_E(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_NWW] = N_W_W(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_SSE] = S_S_E(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_SEE] = S_E_E(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_SSW] = S_S_W(bitboards[white + knight] & ~ all_pins) & target_mask;
  dir_golem[dir_SWW] = S_W_W(bitboards[white + knight] & ~ all_pins) & target_mask;

  dir_golem[dir_NE] |= N_E(bitboards[white + pawn] & ~ (all_pins ^ ant_block)) & bitboards[black] & target_mask;
  dir_golem[dir_NW] |= N_W(bitboards[white + pawn] & ~ (all_pins ^ dia_block)) & bitboards[black] & target_mask;
  dir_golem[dir_N] |= ~bitboards[all] & N(bitboards[white + pawn] & ~ (all_pins ^ ver_block)) & target_mask;
  dir_golem[dir_N] |= (~(bitboards[all] | (bitboards[all] << 8))) & (N(N(bitboards[white + pawn] & ~(all_pins ^ ver_block)))) & target_mask & rank_4;

  dir_golem[dir_EP] = target_mask & EP_rights & (N_E(bitboards[white + pawn] & ~(all_pins ^ ant_block)) | N_W(bitboards[white + pawn] & ~(all_pins ^ dia_block)));

  target_mask = ~(bitboards[white] | ennemy_att);

  dir_golem[dir_N] |= N(bitboards[white + king]) & target_mask;
  dir_golem[dir_S] |= S(bitboards[white + king]) & target_mask;
  dir_golem[dir_W] |= W(bitboards[white + king]) & target_mask;
  dir_golem[dir_E] |= E(bitboards[white + king]) & target_mask;
  dir_golem[dir_NE] |= N_E(bitboards[white + king]) & target_mask;
  dir_golem[dir_NW] |= N_W(bitboards[white + king]) & target_mask;
  dir_golem[dir_SE] |= S_E(bitboards[white + king]) & target_mask;
  dir_golem[dir_SW] |= S_W(bitboards[white + king]) & target_mask;
  //castle
  dir_golem[dir_castle] = 0;
  if(castle_rights[0]){
    if (!((bitboards[all] & 0b110) || (ennemy_att & 0b1110))){
      dir_golem[dir_castle] = 2;
    }
  }
  if(castle_rights[1]){
    if (!((bitboards[all] & 0b1110000) || (ennemy_att & 0b111000))){
      dir_golem[dir_castle] |= 0b100000;
    }
  }
}




//main function that generates the list of all legal moves
std::vector<move> chessboard::get_all_moves(){
  std::vector<move> moves;
  //get_dirgolem();
  int ind = 64;
  for (bitboard bit = one << 63; bit != 0; bit >>= 1){
    ind--;
    if (dir_golem[dir_N] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & S_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      if (m.from_b & bitboards[white + pawn] & rank_7){ m.type = promotion;}
      moves.push_back(m);
    }
    if (dir_golem[dir_S] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & N_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_E] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & W_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_W] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & E_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_NE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & SW_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){
        m.type = capture;
        if (m.from_b & bitboards[white + pawn] & rank_7){ m.type = promo_capture;}
      }
      moves.push_back(m);
    }
    if (dir_golem[dir_NW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & SE_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){
        m.type = capture;
        if (m.from_b & bitboards[white + pawn] & rank_7){ m.type = promo_capture;}
      }
      moves.push_back(m);
    }
    if (dir_golem[dir_SE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NW_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_SW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NE_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_NNE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_NNW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_NEE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_NWW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_SSE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_SEE] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_SSW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_SWW] & bit){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      if (m.to_b & bitboards[black]){ m.type = capture;}
      moves.push_back(m);
    }
    if (dir_golem[dir_EP] & bit){
      if (S_E(bit) & bitboards[white + pawn]){
        move m;
        m.type = en_passant;
        m.to = ind;
        m.from = ind - 9;
        m.to_b = bit;
        m.from_b = bit >> 9;
        m.piece = white + pawn;
        m.target = black + pawn;
        moves.push_back(m);
      }
      if (S_W(bit) & bitboards[white + pawn]){
        move m;
        m.type = en_passant;
        m.to = ind;
        m.from = ind - 7;
        m.to_b = bit;
        m.from_b = bit >> 7;
        m.piece = white + pawn;
        m.target = black + pawn;
        moves.push_back(m);
      }
    }
  }
  if (dir_golem[dir_castle] & 2){
    move m;
    m.type = castle;
    m.from = 3;
    m.to = 1;
    m.from_b = 0b1000;
    m.to_b = 0b10;
    m.piece = white + king;
    moves.push_back(m);
  }
  if (dir_golem[dir_castle] & 0b100000){
    move m;
    m.type = castle;
    m.from = 3;
    m.to = 5;
    m.from_b = 0b1000;
    m.to_b = 0b100000;
    m.piece = white + king;
    moves.push_back(m);
  }
  return moves;
}


















//following functions are used for staged move generation, witch saves a little bit of time during the search
//unfortunatelly this aproach wasn't used in the end but stage 1 and stage 2 moves are used in quiecense search

std::vector<move> chessboard::stage_1_moves(){//pawn captures and promotion
  std::vector<move> moves;
  int ind = 64;
  if (bitboards[white + pawn] & rank_7){//promotion and promo_capture
    for (bitboard bit = one << 63; bit != one << 55; bit >>= 1){
      ind--;
      if (dir_golem[dir_N] & bit){
        if (S(bit) & bitboards[white + pawn]){
          move m;
          m.type = promotion;
          m.from = ind - 8;
          m.to = ind;
          m.from_b = S(bit);
          m.to_b = bit;
          m.piece = white + pawn;
          moves.push_back(m);
        }
      }
      if (dir_golem[dir_NE] & bit){
        if (S_W(bit) & bitboards[white + pawn]){
          move m;
          m.type = promo_capture;
          m.from = ind - 7;
          m.to = ind;
          m.from_b = S_W(bit);
          m.to_b = bit;
          m.piece = white + pawn;
          m.target = board[ind];
          moves.push_back(m);
        }
      }
      if (dir_golem[dir_NW] & bit){
        if (S_E(bit) & bitboards[white + pawn]){
          move m;
          m.type = promo_capture;
          m.from = ind - 9;
          m.to = ind;
          m.from_b = S_E(bit);
          m.to_b = bit;
          m.target = board[ind];
          m.piece = white + pawn;
          moves.push_back(m);
        }
      }
    }
  }
  ind = 56;
  for (bitboard bit = one << 55; bit != 0b1000000000000000; bit >>= 1){
    ind--;
    if (dir_golem[dir_NE] & bit){
      if (S_W(bit) & bitboards[white + pawn]){
        move m;
        m.from = ind - 7;
        m.to = ind;
        m.to_b = bit;
        m.from_b = S_W(bit);
        m.target = board[ind];
        m.piece = white + pawn;
        m.type = capture;
        moves.push_back(m);
      }
    }
    if (dir_golem[dir_NW] & bit){
      if (S_E(bit) & bitboards[white + pawn]){
        move m;
        m.from = ind - 9;
        m.to = ind;
        m.to_b = bit;
        m.from_b = S_E(bit);
        m.target = board[ind];
        m.piece = white + pawn;
        m.type = capture;
        moves.push_back(m);
      }
    }
    if (dir_golem[dir_EP] & bit){
      if (S_W(bit) & bitboards[white + pawn]){
        move m;
        m.from = ind - 7;
        m.to = ind;
        m.to_b = bit;
        m.from_b = S_W(bit);
        m.target = board[ind - 8];
        m.piece = white + pawn;
        m.type = en_passant;
        moves.push_back(m);
      }
      if (S_E(bit) & bitboards[white + pawn]){
        move m;
        m.from = ind - 9;
        m.to = ind;
        m.to_b = bit;
        m.from_b = S_E(bit);
        m.target = board[ind - 8];
        m.piece = white + pawn;
        m.type = en_passant;
        moves.push_back(m);
      }
    }
  }
  return moves;
}



std::vector<move> chessboard::stage_2_moves(){
  std::vector<move> moves;
  int ind = 64;
  for (bitboard bit = one << 63; bit != 0; bit >>= 1){
    ind--;
    if (dir_golem[dir_N] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & S_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_S] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & N_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_E] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & W_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_W] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & E_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_NE] & bit & bitboards[black]){
      if (!(S_W(bit) & bitboards[white + pawn])){
        move m;
        m.to = ind;
        m.to_b = bit;
        m.from_b = bitboards[white] & SW_attacks(bit, bitboards[all]);
        m.from = std::log2(m.from_b);
        m.target = board[m.to];
        m.piece = board[m.from];
        m.type = capture;
        moves.push_back(m);
      }
    }
    if (dir_golem[dir_NW] & bit & bitboards[black]){
      if (!(S_E(bit) & bitboards[white + pawn])){
        move m;
        m.to = ind;
        m.to_b = bit;
        m.from_b = bitboards[white] & SE_attacks(bit, bitboards[all]);
        m.from = std::log2(m.from_b);
        m.target = board[m.to];
        m.piece = board[m.from];
        m.type = capture;
        moves.push_back(m);
      }
    }
    if (dir_golem[dir_SE] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NW_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_SW] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NE_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_NNE] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_NNW] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_NEE] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_NWW] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_SSE] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_SEE] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_SSW] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
    if (dir_golem[dir_SWW] & bit & bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = capture;
      moves.push_back(m);
    }
  }
  return moves;
}




std::vector<move> chessboard::stage_3_moves(){
  std::vector<move> moves;
  if (dir_golem[dir_castle] & 2){
    move m;
    m.type = castle;
    m.from = 3;
    m.to = 1;
    m.from_b = 0b1000;
    m.to_b = 0b10;
    m.piece = white + king;
    moves.push_back(m);
  }
  if (dir_golem[dir_castle] & 0b100000){
    move m;
    m.type = castle;
    m.from = 3;
    m.to = 5;
    m.from_b = 0b1000;
    m.to_b = 0b100000;
    m.piece = white + king;
    moves.push_back(m);
  }
  return moves;
}


std::vector<move> chessboard::stage_4_moves(){
  std::vector<move> moves;
  int ind = 64;
  for (bitboard bit = one << 63; bit != 0; bit >>= 1){
    ind--;
    if (dir_golem[dir_N] & bit & ~bitboards[black]){
      if (!(S(bit & rank_8) & bitboards[white + pawn])){
        move m;
        m.to = ind;
        m.to_b = bit;
        m.from_b = bitboards[white] & S_attacks(bit, bitboards[all]);
        m.from = std::log2(m.from_b);
        m.target = board[m.to];
        m.piece = board[m.from];
        m.type = quiet;
        moves.push_back(m);
      }
    }
    if (dir_golem[dir_S] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & N_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_E] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & W_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_W] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & E_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & SW_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & SE_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NW_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = bitboards[white] & NE_attacks(bit, bitboards[all]);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = board[m.from];
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NNE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NNW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_S_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NEE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_NWW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = S_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SSE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SEE] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_W_W(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SSW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_N_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
    if (dir_golem[dir_SWW] & bit & ~bitboards[black]){
      move m;
      m.to = ind;
      m.to_b = bit;
      m.from_b = N_E_E(bit);
      m.from = std::log2(m.from_b);
      m.target = board[m.to];
      m.piece = 5;
      m.type = quiet;
      moves.push_back(m);
    }
  }
  return moves;
}





//following function search only for the move from the transposition table, witch is supposed to be the best one

move chessboard::get_hash_move(bitboard hash_move){
  if (dir_golem[dir_N] & hash_move){
    if (S_attacks(hash_move & dir_golem[dir_N], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_N] & hash_move;
      m.from_b = S_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[white + pawn] & rank_7 & m.from_b){ m.type = promotion;}
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_N] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_S] & hash_move){
    if (N_attacks(hash_move & dir_golem[dir_S], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_S] & hash_move;
      m.from_b = N_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_S] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_E] & hash_move){
    if (W_attacks(hash_move & dir_golem[dir_E], bitboards[all]) & hash_move){
      if (*(reached_pos.end() - 1) == 7248382528037906625){ std::cout << "yo" << std::endl;}
      move m;
      m.to_b = dir_golem[dir_E] & hash_move;
      m.from_b = W_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_E] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_W] & hash_move){
    if (E_attacks(hash_move & dir_golem[dir_W], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_W] & hash_move;
      m.from_b = E_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_W] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_NE] & hash_move){
    if (SW_attacks(hash_move & dir_golem[dir_NE], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NE] & hash_move;
      m.from_b = SW_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[white + pawn] & rank_7 & m.from_b){
        m.type = promo_capture;
        m.target = board[m.to];
      }
      else if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NE] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_NW] & hash_move){
    if (SE_attacks(hash_move & dir_golem[dir_NW], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NW] & hash_move;
      m.from_b = SE_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[white + pawn] & rank_7 & m.from_b){
        m.type = promo_capture;
        m.target = board[m.to];
      }
      else if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NW] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SW] & hash_move){
    if (NE_attacks(hash_move & dir_golem[dir_SW], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SW] & hash_move;
      m.from_b = NE_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SW] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SE] & hash_move){
    if (NW_attacks(hash_move & dir_golem[dir_SE], bitboards[all]) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SE] & hash_move;
      m.from_b = NW_attacks(hash_move, bitboards[all]) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = board[m.from];
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SE] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SSW] & hash_move){
    if (N_N_E(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SSW] & hash_move;
      m.from_b = N_N_E(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SSW] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SWW] & hash_move){
    if (N_E_E(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SWW] & hash_move;
      m.from_b = N_E_E(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SWW] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_NWW] & hash_move){
    if (S_E_E(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NWW] & hash_move;
      m.from_b = S_E_E(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NWW] ^= m.to_b;
      return m;
    }
  }




  if (dir_golem[dir_NNW] & hash_move){
    if (S_S_E(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NNW] & hash_move;
      m.from_b = S_S_E(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NNW] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_NNE] & hash_move){
    if (S_S_W(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NNE] & hash_move;
      m.from_b = S_S_W(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NNE] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_NEE] & hash_move){
    if (S_W_W(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_NEE] & hash_move;
      m.from_b = S_W_W(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_NEE] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SEE] & hash_move){
    if (N_W_W(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SEE] & hash_move;
      m.from_b = N_W_W(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SEE] ^= m.to_b;
      return m;
    }
  }

  if (dir_golem[dir_SSE] & hash_move){
    if (N_N_W(hash_move) & hash_move){
      move m;
      m.to_b = dir_golem[dir_SSE] & hash_move;
      m.from_b = N_N_W(hash_move) & hash_move;
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = 5;
      m.type = quiet;
      if (bitboards[black] & m.to_b){
        m.type = capture;
        m.target = board[m.to];
      }
      dir_golem[dir_SSE] ^= m.to_b;
      return m;
    }
  }


  if (dir_golem[dir_EP] & hash_move){
    if (S_W(hash_move) & hash_move){
      move m;
      m.type = en_passant;
      m.to_b = dir_golem[dir_EP];
      m.from_b = S_W(m.to_b);
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = white + pawn;
      m.target = black + pawn;
      return m;
    }
    if (S_E(hash_move) & hash_move){
      move m;
      m.type = en_passant;
      m.to_b = dir_golem[dir_EP];
      m.from_b = S_E(m.to_b);
      m.from = std::log2(m.from_b);
      m.to = std::log2(m.to_b);
      m.piece = white + pawn;
      m.target = black + pawn;
      return m;
    }
  }

  if (hash_move & dir_golem[dir_castle]){
    if (!(hash_move ^ 0b1010)){
      move m;
      m.type = castle;
      m.from = 3;
      m.to = 1;
      m.from_b = 0b1000;
      m.to_b = 0b10;
      m.piece = white + king;
      return m;
    }
    if (!(hash_move ^ 0b101000)){
      move m;
      m.type = castle;
      m.from = 3;
      m.to = 5;
      m.from_b = 0b1000;
      m.to_b = 0b100000;
      m.piece = white + king;
      return m;
    }
  }
  move m;
  m.from_b = 0;
  std::cout << "error : TTmove not found" << std::endl;
  display(true);
  display_bitboard(hash_move);
  return m;
}
