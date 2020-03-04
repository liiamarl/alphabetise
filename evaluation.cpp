#include "bit_function.hpp"
#include <iostream>
#include "structs.hpp"
#include "chessboard.hpp"
#include <array>
#include <cstdlib>
#include <ctime>
#include <string>
#include "search.hpp"

//all values are in centipawn

//kinda heavy to rewrite almost the same function 4 times but whatever





int chessboard::eval_opening(std::array<int, 25> const& coefs){//coefs were used for genetic optimisation
  int eval = ST_eval + coefs[o_turn];
  get_att_table(true);
  get_att_table(false);
  attack_table[white + knight] &= ~attack_table[black + pawn];//squares defended by pawns aren't counted in mobility
  attack_table[white + bishop] &= ~attack_table[black + pawn];
  attack_table[black + knight] &= ~attack_table[white + pawn];
  attack_table[black + bishop] &= ~attack_table[white + pawn];


  //minor pieces board control
  eval += coefs[o_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & ext_center);
  eval += coefs[o_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & black_camp);


  eval -= coefs[o_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & ext_center);
  eval -= coefs[o_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & white_camp);

  if(popcount(bitboards[white + bishop]) == 2){ eval += coefs[o_bishop_pair];}
  if(popcount(bitboards[black + bishop]) == 2){ eval -= coefs[o_bishop_pair];}




  bitboard white_outpost = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn])));
  bitboard black_outpost = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn])));
  bitboard white_weak_pawn = ~(W(bitboards[white + pawn]) | E(bitboards[white + pawn])) & bitboards[white + pawn] & black_outpost & S(attack_table[black + pawn] | bitboards[all]);
  bitboard black_weak_pawn = ~(W(bitboards[black + pawn]) | E(bitboards[black + pawn])) &  bitboards[black + pawn] & white_outpost & N(attack_table[white + pawn] | bitboards[all]);
  white_outpost &= N(ext_center);
  black_outpost &= S(ext_center);
  white_outpost &= attack_table[white + pawn];
  black_outpost &= attack_table[black + pawn];
  bitboard white_passed = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn]) | bitboards[black + pawn]));
  bitboard black_passed = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn]) | bitboards[white + pawn]));
  white_passed &= bitboards[white + pawn];
  black_passed &= bitboards[black + pawn];

  eval += coefs[o_outpost] * popcount(white_outpost & (bitboards[white + knight] | bitboards[white + bishop]));
  eval -= coefs[o_outpost] * popcount(black_outpost & (bitboards[black + knight] | bitboards[black + bishop]));



  eval += coefs[o_passed] * popcount(white_passed & bitboards[white + pawn]);
  eval -= coefs[o_passed] * popcount(black_passed & bitboards[black + pawn]);
//protected passed pawn
  eval += coefs[o_passed] * popcount(white_passed & bitboards[white + pawn] & attack_table[white + pawn]);
  eval -= coefs[o_passed] * popcount(black_passed & bitboards[black + pawn] & attack_table[white + pawn]);




  //king safety

  bitboard shield_zone = bitboards[white + king] | W(bitboards[white + king]) | E(bitboards[white + king]);
  shield_zone |= N(shield_zone);
  shield_zone |= N(shield_zone);
  int shield = popcount(shield_zone & bitboards[white + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(north_line(bitboards[white + king]) & bitboards[white + pawn])){ shield -= 2;}
  if (bitboards[white + king] & (a_file | b_file | c_file)){
    if (!(north_east_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  if (bitboards[white + king] & (f_file | g_file | h_file)){
    if (!(north_west_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  eval += coefs[o_shield] * shield;

  eval -= 3 * coefs[o_tropism] * popcount(attack_table[black + queen] & shield_zone);
  eval -= 2 * coefs[o_tropism] * popcount(attack_table[black + rook] & shield_zone);
  eval -= coefs[o_tropism] * popcount((attack_table[black + bishop] | attack_table[black + knight]) & shield_zone);

  int storm = popcount(N(shield_zone) & bitboards[black + pawn]);
  eval -= storm * coefs[o_storm];


  shield_zone = bitboards[black + king] | W(bitboards[black + king]) | E(bitboards[black + king]);
  shield_zone |= S(shield_zone);
  shield_zone |= S(shield_zone);
  shield = popcount(shield_zone & bitboards[black + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(south_line(bitboards[black + king]) & bitboards[black + pawn])){ shield -= 2;}
  if (bitboards[black + king] & (a_file | b_file | c_file)){
    if (!(south_east_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  if (bitboards[black + king] & (f_file | g_file | h_file)){
    if (!(south_west_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  eval -= coefs[o_shield] * shield;

  eval += 3 * coefs[o_tropism] * popcount(attack_table[white + queen] & shield_zone);
  eval += 2 * coefs[o_tropism] * popcount(attack_table[white + rook] & shield_zone);
  eval += coefs[o_tropism] * popcount((attack_table[white + bishop] | attack_table[white + knight]) & shield_zone);

  storm = popcount(S(shield_zone) & bitboards[white + pawn]);
  eval += storm * coefs[o_storm];

  //std::cout << "king safety " << eval << std::endl;

  bitboard white_open = bitboards[white + pawn];
  white_open |= N(white_open);
  white_open |= N(N(white_open));
  white_open |= N(N(N(white_open)));
  white_open |= S(white_open);
  white_open |= S(S(white_open));
  white_open |= S(S(S(white_open)));
  white_open = ~white_open;

  bitboard black_open = bitboards[black + pawn];
  black_open |= N(black_open);
  black_open |= N(N(black_open));
  black_open |= N(N(N(black_open)));
  black_open |= S(black_open);
  black_open |= S(S(black_open));
  black_open |= S(S(S(black_open)));
  black_open = ~black_open;

  //rooks evaluation

  if (N_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){//doubled rooks
    eval += coefs[o_doubled_rooks];
    if (bitboards[white + rook] & white_open){
      eval += coefs[o_doubled_rooks];
      if (bitboards[white + rook] & black_open){
        eval += 2 * coefs[o_doubled_rooks];
      }
    }
  }

  if (N_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){//doubled rooks
    eval -= coefs[o_doubled_rooks];
    if (bitboards[black + rook] & black_open){
      eval -= coefs[o_doubled_rooks];
      if (bitboards[black + rook] & white_open){
        eval -= 2 * coefs[o_doubled_rooks];
      }
    }
  }

  if (E_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){ eval += coefs[o_connected_rooks];}
  if (E_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){ eval -= coefs[o_connected_rooks];}

  if (north_line(bitboards[white + rook]) & bitboards[black + queen]){eval += coefs[o_queen_rook];}
  if (south_line(bitboards[black + rook]) & bitboards[white + queen]){eval -= coefs[o_queen_rook];}

  //rooks on semi open file
  eval += coefs[o_open_rook] * popcount(bitboards[white + rook] & white_open);
  eval -= coefs[o_open_rook] * popcount(bitboards[black + rook] & black_open);

  //rook on open file

  eval += coefs[o_open_rook] * popcount(bitboards[white + rook] & white_open & black_open);
  eval -= coefs[o_open_rook] * popcount(bitboards[black + rook] & white_open & black_open);

  //std::cout << "rooks " << eval << std::endl;

  //pawn_structure
  eval -= coefs[o_weak_pawn] * popcount(white_weak_pawn);
  eval += coefs[o_weak_pawn] * popcount(black_weak_pawn);
  eval += coefs[o_pawn_co] * popcount((bitboards[white + pawn] & ~rank_2) & (E(bitboards[white + pawn]) | W(bitboards[white + pawn]) | S_E(bitboards[white + pawn]) | S_W(bitboards[white + pawn])));
  eval -= coefs[o_pawn_co] * popcount((bitboards[black + pawn] & ~rank_7) & (E(bitboards[black + pawn]) | W(bitboards[black + pawn]) | S_E(bitboards[black + pawn]) | S_W(bitboards[black + pawn])));

  eval -= coefs[o_doubled] * popcount(bitboards[white + pawn] & south_line(bitboards[white + pawn]));
  eval += coefs[o_doubled] * popcount(bitboards[black + pawn] & north_line(bitboards[black + pawn]));

  eval -= coefs[o_blocked] * popcount(bitboards[all] & N(bitboards[white + pawn] & rank_2 & (c_file | d_file | e_file | f_file)));

  eval += coefs[o_blocked] * popcount(bitboards[all] & S(bitboards[black + pawn] & rank_7 & (c_file | d_file | e_file | f_file)));

  bitboard iso = bitboards[white + pawn];
  iso |= (north_line(iso) | south_line(iso));
  iso = ~(E(iso) | W(iso));
  iso = bitboards[white + pawn] & iso;
  eval -= coefs[o_isolated] * popcount(iso);

  iso = bitboards[black + pawn];
  iso |= (north_line(iso) | south_line(iso));
  iso = ~(E(iso) | W(iso));
  iso = bitboards[black + pawn] & iso;
  eval += coefs[o_isolated] * popcount(iso);

  //std::cout << "pawn_structure " << eval << std::endl;

  eval += coefs[o_pawn_knight] * (popcount(bitboards[white + knight]) - popcount(bitboards[black + knight])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval += coefs[o_pawn_rook] * (popcount(bitboards[white + rook]) - popcount(bitboards[black + rook])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval -= coefs[o_undefended] * popcount((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white]);
  eval += coefs[o_undefended] * popcount((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black]);

  eval += coefs[o_bishop_knight] * popcount( bitboards[white + bishop] & ( S(S(S(bitboards[black + knight]))) | N(N(N(bitboards[black + knight]))) | E(E(E(bitboards[black + knight]))) | W(W(W(bitboards[black + knight])))));
  eval -= coefs[o_bishop_knight] * popcount( bitboards[black + bishop] & ( S(S(S(bitboards[white + knight]))) | N(N(N(bitboards[white + knight]))) | E(E(E(bitboards[white + knight]))) | W(W(W(bitboards[white + knight])))));

  //attack potential evaluation
  bitboard targets = (bitboards[black + bishop] & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[o_knight_att] * popcount(attack_table[white + knight] & targets);

  targets = (bitboards[black + knight] & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[o_bishop_att] * popcount(attack_table[white + bishop] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[o_rook_att] * popcount(attack_table[rook + white] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop] | bitboards[black + rook]) & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[o_queen_att] * popcount(attack_table[queen + white] & targets);


  targets = (bitboards[white + bishop] & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[o_knight_att] * popcount(attack_table[black + knight] & targets);

  targets = (bitboards[white + knight] & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[o_bishop_att] * popcount(attack_table[black + bishop] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[o_rook_att] * popcount(attack_table[black + rook] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop] | bitboards[white + rook]) & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[o_queen_att] * popcount(attack_table[queen + black] & targets);

  //std::cout << "pieces interaction " << eval << std::endl;

  return eval;


}
















int chessboard::eval_midgame(std::array<int, 25> const& coefs){
  int eval = ST_eval + coefs[m_turn];
  get_att_table(true);
  get_att_table(false);


  attack_table[white + knight] &= ~attack_table[black + pawn];//squares defended by pawns aren't counted in mobility
  attack_table[white + bishop] &= ~attack_table[black + pawn];
  attack_table[black + knight] &= ~attack_table[white + pawn];
  attack_table[black + bishop] &= ~attack_table[white + pawn];


  //minor pieces board control
  eval += coefs[m_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & ext_center);
  eval += coefs[m_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & black_camp);


  eval -= coefs[m_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & ext_center);
  eval -= coefs[m_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & white_camp);

  if(popcount(bitboards[white + bishop]) == 2){ eval += coefs[m_bishop_pair];}
  if(popcount(bitboards[black + bishop]) == 2){ eval -= coefs[m_bishop_pair];}


  bitboard white_outpost = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn])));
  bitboard black_outpost = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn])));
  bitboard white_weak_pawn = ~(W(bitboards[white + pawn]) | E(bitboards[white + pawn])) & bitboards[white + pawn] & black_outpost & S(attack_table[black + pawn] | bitboards[all]);
  bitboard black_weak_pawn = ~(W(bitboards[black + pawn]) | E(bitboards[black + pawn])) &  bitboards[black + pawn] & white_outpost & N(attack_table[white + pawn] | bitboards[all]);
  white_outpost &= N(ext_center);
  black_outpost &= S(ext_center);
  white_outpost &= attack_table[white + pawn];
  black_outpost &= attack_table[black + pawn];
  bitboard white_passed = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn]) | bitboards[black + pawn]));
  bitboard black_passed = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn]) | bitboards[white + pawn]));
  white_passed &= bitboards[white + pawn];
  black_passed &= bitboards[black + pawn];



  eval += coefs[m_outpost] * popcount(white_outpost & (bitboards[white + knight] | bitboards[white + bishop]));
  eval -= coefs[m_outpost] * popcount(black_outpost & (bitboards[black + knight] | bitboards[black + bishop]));

  //std::cout << "minor pieces " << eval << std::endl;

  eval += coefs[m_passed] * popcount(white_passed & bitboards[white + pawn]);
  eval -= coefs[m_passed] * popcount(black_passed & bitboards[black + pawn]);
//protected passed pawn
  eval += coefs[m_passed] * popcount(white_passed & bitboards[white + pawn] & attack_table[white + pawn]);
  eval -= coefs[m_passed] * popcount(black_passed & bitboards[black + pawn] & attack_table[white + pawn]);

  //std::cout << "passed pawn " << eval << std::endl;


  //king safety

  bitboard shield_zone = bitboards[white + king] | W(bitboards[white + king]) | E(bitboards[white + king]);
  shield_zone |= N(shield_zone);
  shield_zone |= N(shield_zone);
  int shield = popcount(shield_zone & bitboards[white + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(north_line(bitboards[white + king]) & bitboards[white + pawn])){ shield -= 2;}
  if (bitboards[white + king] & (a_file | b_file | c_file)){
    if (!(north_east_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  if (bitboards[white + king] & (f_file | g_file | h_file)){
    if (!(north_west_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  eval += coefs[m_shield] * shield;

  eval -= 3 * coefs[m_tropism] * popcount(attack_table[black + queen] & shield_zone);
  eval -= 2 * coefs[m_tropism] * popcount(attack_table[black + rook] & shield_zone);
  eval -= coefs[m_tropism] * popcount((attack_table[black + bishop] | attack_table[black + knight]) & shield_zone);

  int storm = popcount(N(shield_zone) & bitboards[black + pawn]);
  eval -= storm * coefs[m_storm];


  shield_zone = bitboards[black + king] | W(bitboards[black + king]) | E(bitboards[black + king]);
  shield_zone |= S(shield_zone);
  shield_zone |= S(shield_zone);
  shield = popcount(shield_zone & bitboards[black + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(south_line(bitboards[black + king]) & bitboards[black + pawn])){ shield -= 2;}
  if (bitboards[black + king] & (a_file | b_file | c_file)){
    if (!(south_east_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  if (bitboards[black + king] & (f_file | g_file | h_file)){
    if (!(south_west_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  eval -= coefs[m_shield] * shield;

  eval += 3 * coefs[m_tropism] * popcount(attack_table[white + queen] & shield_zone);
  eval += 2 * coefs[m_tropism] * popcount(attack_table[white + rook] & shield_zone);
  eval += coefs[m_tropism] * popcount((attack_table[white + bishop] | attack_table[white + knight]) & shield_zone);

  storm = popcount(S(shield_zone) & bitboards[white + pawn]);
  eval += storm * coefs[m_storm];

  //std::cout << "king safety " << eval << std::endl;

  bitboard white_open = bitboards[white + pawn];
  white_open |= N(white_open);
  white_open |= N(N(white_open));
  white_open |= N(N(N(white_open)));
  white_open |= S(white_open);
  white_open |= S(S(white_open));
  white_open |= S(S(S(white_open)));
  white_open = ~white_open;

  bitboard black_open = bitboards[black + pawn];
  black_open |= N(black_open);
  black_open |= N(N(black_open));
  black_open |= N(N(N(black_open)));
  black_open |= S(black_open);
  black_open |= S(S(black_open));
  black_open |= S(S(S(black_open)));
  black_open = ~black_open;

  //rooks evaluation

  if (N_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){//doubled rooks
    eval += coefs[m_doubled_rooks];
    if (bitboards[white + rook] & white_open){
      eval += coefs[m_doubled_rooks];
      if (bitboards[white + rook] & black_open){
        eval += 2 * coefs[m_doubled_rooks];
      }
    }
  }

  if (N_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){//doubled rooks
    eval -= coefs[m_doubled_rooks];
    if (bitboards[black + rook] & black_open){
      eval -= coefs[m_doubled_rooks];
      if (bitboards[black + rook] & white_open){
        eval -= 2 * coefs[m_doubled_rooks];
      }
    }
  }

  if (E_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){ eval += coefs[m_connected_rooks];}
  if (E_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){ eval -= coefs[m_connected_rooks];}

  if (north_line(bitboards[white + rook]) & bitboards[black + queen]){eval += coefs[m_queen_rook];}
  if (south_line(bitboards[black + rook]) & bitboards[white + queen]){eval -= coefs[m_queen_rook];}

  //rooks on semi open file
  eval += coefs[m_open_rook] * popcount(bitboards[white + rook] & white_open);
  eval -= coefs[m_open_rook] * popcount(bitboards[black + rook] & black_open);

  //rook on open file

  eval += coefs[m_open_rook] * popcount(bitboards[white + rook] & white_open & black_open);
  eval -= coefs[m_open_rook] * popcount(bitboards[black + rook] & white_open & black_open);


  //std::cout << "rooks " << eval << std::endl;

  //pawn_structure
  eval -= coefs[m_weak_pawn] * popcount(white_weak_pawn);
  eval += coefs[m_weak_pawn] * popcount(black_weak_pawn);
  eval += coefs[m_pawn_co] * popcount((bitboards[white + pawn] & ~rank_2) & (E(bitboards[white + pawn]) | W(bitboards[white + pawn]) | S_E(bitboards[white + pawn]) | S_W(bitboards[white + pawn])));
  eval -= coefs[m_pawn_co] * popcount((bitboards[black + pawn] & ~rank_7) & (E(bitboards[black + pawn]) | W(bitboards[black + pawn]) | S_E(bitboards[black + pawn]) | S_W(bitboards[black + pawn])));

  eval -= coefs[m_doubled] * popcount(bitboards[white + pawn] & south_line(bitboards[white + pawn]));
  eval += coefs[m_doubled] * popcount(bitboards[black + pawn] & north_line(bitboards[black + pawn]));

  eval -= coefs[m_blocked] * popcount(bitboards[all] & N(bitboards[white + pawn] & rank_2 & (c_file | d_file | e_file | f_file)));

  eval += coefs[m_blocked] * popcount(bitboards[all] & S(bitboards[black + pawn] & rank_7 & (c_file | d_file | e_file | f_file)));

  bitboard iso = bitboards[white + pawn];
  iso |= (north_line(iso) | south_line(iso));
  iso = ~(E(iso) | W(iso));
  iso = bitboards[white + pawn] & iso;
  eval -= coefs[m_isolated] * popcount(iso);

  iso = bitboards[black + pawn];
  iso |= (north_line(iso) | south_line(iso));
  iso = ~(E(iso) | W(iso));
  iso = bitboards[black + pawn] & iso;
  eval += coefs[m_isolated] * popcount(iso);

  //std::cout << "pawn structure " << eval << std::endl;



  eval += coefs[m_pawn_knight] * (popcount(bitboards[white + knight]) - popcount(bitboards[black + knight])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval += coefs[m_pawn_rook] * (popcount(bitboards[white + rook]) - popcount(bitboards[black + rook])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval -= coefs[m_undefended] * popcount((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white]);
  eval += coefs[m_undefended] * popcount((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black]);

  eval += coefs[m_bishop_knight] * popcount( bitboards[white + bishop] & ( S(S(S(bitboards[black + knight]))) | N(N(N(bitboards[black + knight]))) | E(E(E(bitboards[black + knight]))) | W(W(W(bitboards[black + knight])))));
  eval -= coefs[m_bishop_knight] * popcount( bitboards[black + bishop] & ( S(S(S(bitboards[white + knight]))) | N(N(N(bitboards[white + knight]))) | E(E(E(bitboards[white + knight]))) | W(W(W(bitboards[white + knight])))));

  //attack potential evaluation
  bitboard targets = (bitboards[black + bishop] & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[m_knight_att] * popcount(attack_table[white + knight] & targets);

  targets = (bitboards[black + knight] & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[m_bishop_att] * popcount(attack_table[white + bishop] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[m_rook_att] * popcount(attack_table[rook + white] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop] | bitboards[black + rook]) & ~attack_table[black + pawn]) | black_weak_pawn;
  eval += coefs[m_queen_att] * popcount(attack_table[queen + white] & targets);


  targets = (bitboards[white + bishop] & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[m_knight_att] * popcount(attack_table[black + knight] & targets);

  targets = (bitboards[white + knight] & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[m_bishop_att] * popcount(attack_table[black + bishop] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[m_rook_att] * popcount(attack_table[black + rook] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop] | bitboards[white + rook]) & ~attack_table[white + pawn]) | white_weak_pawn;
  eval -= coefs[m_queen_att] * popcount(attack_table[queen + black] & targets);


  //std::cout << "pieces interaction " << eval << std::endl;

  return eval;
}



















int chessboard::eval_early_end(std::array<int, 27> const& coefs){
  int eval = ST_eval + coefs[ee_turn];
  //std::cout << "squares tables " << eval << std::endl;
  get_att_table(true);
  get_att_table(false);


  attack_table[white + knight] &= ~attack_table[black + pawn];//squares defended by pawns aren't counted in mobility
  attack_table[white + bishop] &= ~attack_table[black + pawn];
  attack_table[black + knight] &= ~attack_table[white + pawn];
  attack_table[black + bishop] &= ~attack_table[white + pawn];


  //pieces board control
  eval += coefs[ee_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & ext_center);
  eval += coefs[ee_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & black_camp);


  eval -= coefs[ee_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & ext_center);
  eval -= coefs[ee_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & white_camp);

  eval += coefs[ee_rook_control] * popcount(attack_table[white + rook] & (ext_center | black_camp));
  eval -= coefs[ee_rook_control] * popcount(attack_table[black + rook] & (ext_center | white_camp));

  if(popcount(bitboards[white + bishop]) == 2){ eval += coefs[ee_bishop_pair];}
  if(popcount(bitboards[black + bishop]) == 2){ eval -= coefs[ee_bishop_pair];}


  bitboard white_outpost = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn])));
  bitboard black_outpost = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn])));
  bitboard white_weak_pawn = ~(W(bitboards[white + pawn]) | E(bitboards[white + pawn])) & bitboards[white + pawn] & black_outpost & S(attack_table[black + pawn] | bitboards[all]);
  bitboard black_weak_pawn = ~(W(bitboards[black + pawn]) | E(bitboards[black + pawn])) &  bitboards[black + pawn] & white_outpost & N(attack_table[white + pawn] | bitboards[all]);
  white_outpost &= N(ext_center);
  black_outpost &= S(ext_center);
  white_outpost &= attack_table[white + pawn];
  black_outpost &= attack_table[black + pawn];
  bitboard white_passed = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn]) | bitboards[black + pawn]));
  bitboard black_passed = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn]) | bitboards[white + pawn]));
  white_passed &= bitboards[white + pawn];
  black_passed &= bitboards[black + pawn];

  eval += coefs[ee_outpost] * popcount(white_outpost & (bitboards[white + knight] | bitboards[white + bishop]));
  eval -= coefs[ee_outpost] * popcount(black_outpost & (bitboards[black + knight] | bitboards[black + bishop]));

  //std::cout << "minor pieces " << eval << std::endl;




  //king safety

  bitboard shield_zone = bitboards[white + king] | W(bitboards[white + king]) | E(bitboards[white + king]);
  shield_zone |= N(shield_zone);
  shield_zone |= N(shield_zone);
  int shield = popcount(shield_zone & bitboards[white + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(north_line(bitboards[white + king]) & bitboards[white + pawn])){ shield -= 2;}
  if (bitboards[white + king] & (a_file | b_file | c_file)){
    if (!(north_east_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  if (bitboards[white + king] & (f_file | g_file | h_file)){
    if (!(north_west_line(bitboards[white + king]) & (bitboards[white + pawn] | bitboards[white + bishop]))){ shield--;}
  }
  eval += coefs[ee_shield] * shield;

  eval -= 3 * coefs[ee_tropism] * popcount(attack_table[black + queen] & shield_zone);
  eval -= 2 * coefs[ee_tropism] * popcount(attack_table[black + rook] & shield_zone);
  eval -= coefs[ee_tropism] * popcount((attack_table[black + bishop] | attack_table[black + knight]) & shield_zone);




  shield_zone = bitboards[black + king] | W(bitboards[black + king]) | E(bitboards[black + king]);
  shield_zone |= S(shield_zone);
  shield_zone |= S(shield_zone);
  shield = popcount(shield_zone & bitboards[black + pawn]);
  //checking if king's file and main diagonale are covered
  if (!(south_line(bitboards[black + king]) & bitboards[black + pawn])){ shield -= 2;}
  if (bitboards[black + king] & (a_file | b_file | c_file)){
    if (!(south_east_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  if (bitboards[black + king] & (f_file | g_file | h_file)){
    if (!(south_west_line(bitboards[black + king]) & (bitboards[black + pawn] | bitboards[black + bishop]))){ shield--;}
  }
  eval -= coefs[ee_shield] * shield;

  eval += 3 * coefs[ee_tropism] * popcount(attack_table[white + queen] & shield_zone);
  eval += 2 * coefs[ee_tropism] * popcount(attack_table[white + rook] & shield_zone);
  eval += coefs[ee_tropism] * popcount((attack_table[white + bishop] | attack_table[white + knight]) & shield_zone);


  //std::cout << "king safety " << eval << std::endl;


  bitboard white_open = bitboards[white + pawn];
  white_open |= N(white_open);
  white_open |= N(N(white_open));
  white_open |= N(N(N(white_open)));
  white_open |= S(white_open);
  white_open |= S(S(white_open));
  white_open |= S(S(S(white_open)));
  white_open = ~white_open;

  bitboard black_open = bitboards[black + pawn];
  black_open |= N(black_open);
  black_open |= N(N(black_open));
  black_open |= N(N(N(black_open)));
  black_open |= S(black_open);
  black_open |= S(S(black_open));
  black_open |= S(S(S(black_open)));
  black_open = ~black_open;

  //rooks evaluation

  if (N_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){//doubled rooks
    eval += coefs[ee_doubled_rooks];
    if (bitboards[white + rook] & white_open){
      eval += coefs[ee_doubled_rooks];
      if (bitboards[white + rook] & black_open){
        eval += 2 * coefs[ee_doubled_rooks];
      }
    }
  }

  if (N_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){//doubled rooks
    eval -= coefs[ee_doubled_rooks];
    if (bitboards[black + rook] & black_open){
      eval -= coefs[ee_doubled_rooks];
      if (bitboards[black + rook] & white_open){
        eval -= 2 * coefs[ee_doubled_rooks];
      }
    }
  }

  if (E_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){ eval += coefs[ee_connected_rooks];}
  if (E_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){ eval -= coefs[ee_connected_rooks];}



  //rooks on semi open file
  eval += coefs[ee_open_rook] * popcount(bitboards[white + rook] & white_open);
  eval -= coefs[ee_open_rook] * popcount(bitboards[black + rook] & black_open);

  //rook on open file

  eval += coefs[ee_open_rook] * popcount(bitboards[white + rook] & white_open & black_open);
  eval -= coefs[ee_open_rook] * popcount(bitboards[black + rook] & white_open & black_open);

  //std::cout << "rooks " << eval << std::endl;

  //pawn_structure
  eval -= coefs[ee_weak_pawn] * popcount(white_weak_pawn);
  eval += coefs[ee_weak_pawn] * popcount(black_weak_pawn);
  eval += coefs[ee_pawn_co] * popcount((bitboards[white + pawn] & ~rank_2) & (E(bitboards[white + pawn]) | W(bitboards[white + pawn]) | S_E(bitboards[white + pawn]) | S_W(bitboards[white + pawn])));
  eval -= coefs[ee_pawn_co] * popcount((bitboards[black + pawn] & ~rank_7) & (E(bitboards[black + pawn]) | W(bitboards[black + pawn]) | S_E(bitboards[black + pawn]) | S_W(bitboards[black + pawn])));

  eval -= coefs[ee_doubled] * popcount(bitboards[white + pawn] & south_line(bitboards[white + pawn]));
  eval += coefs[ee_doubled] * popcount(bitboards[black + pawn] & north_line(bitboards[black + pawn]));

  eval -= coefs[ee_blocked] * popcount(bitboards[all] & N(bitboards[white + pawn] & rank_2 & (c_file | d_file | e_file | f_file)));

  eval += coefs[ee_blocked] * popcount(bitboards[all] & S(bitboards[black + pawn] & rank_7 & (c_file | d_file | e_file | f_file)));

  bitboard iso_white = bitboards[white + pawn];
  iso_white |= (north_line(iso_white) | south_line(iso_white));
  iso_white = ~(E(iso_white) | W(iso_white));
  iso_white = bitboards[white + pawn] & iso_white;
  eval -= coefs[ee_isolated] * popcount(iso_white);

  bitboard iso_black = bitboards[black + pawn];
  iso_black |= (north_line(iso_black) | south_line(iso_black));
  iso_black = ~(E(iso_black) | W(iso_black));
  iso_black = bitboards[black + pawn] & iso_black;
  eval += coefs[ee_isolated] * popcount(iso_black);

  //std::cout << "pawn_structure " << eval << std::endl;

  eval += coefs[ee_pawn_knight] * (popcount(bitboards[white + knight]) - popcount(bitboards[black + knight])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval += coefs[ee_pawn_rook] * (popcount(bitboards[white + rook]) - popcount(bitboards[black + rook])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval -= coefs[ee_undefended] * popcount((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white]);
  eval += coefs[ee_undefended] * popcount((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black]);

  eval += coefs[ee_bishop_knight] * popcount( bitboards[white + bishop] & ( S(S(S(bitboards[black + knight]))) | N(N(N(bitboards[black + knight]))) | E(E(E(bitboards[black + knight]))) | W(W(W(bitboards[black + knight])))));
  eval -= coefs[ee_bishop_knight] * popcount( bitboards[black + bishop] & ( S(S(S(bitboards[white + knight]))) | N(N(N(bitboards[white + knight]))) | E(E(E(bitboards[white + knight]))) | W(W(W(bitboards[white + knight])))));

  //std::cout << "pieces interaction " << eval << std::endl;

  //attack potential evaluation
  bitboard targets = (bitboards[black + bishop] & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[ee_knight_att] * popcount(attack_table[white + knight] & targets);

  targets = (bitboards[black + knight] & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[ee_bishop_att] * popcount(attack_table[white + bishop] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[ee_rook_att] * popcount(attack_table[rook + white] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop] | bitboards[black + rook]) & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[ee_queen_att] * popcount(attack_table[queen + white] & targets);

  eval += coefs[ee_king_att] * popcount(attack_table[white + king] & targets);


  targets = (bitboards[white + bishop] & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_knight_att] * popcount(attack_table[black + knight] & targets);

  targets = (bitboards[white + knight] & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_bishop_att] * popcount(attack_table[black + bishop] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_rook_att] * popcount(attack_table[black + rook] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop] | bitboards[white + rook]) & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_queen_att] * popcount(attack_table[queen + black] & targets);

  eval -= coefs[ee_king_att] * popcount(attack_table[black + king] & targets);

  //std::cout << "attacks " << eval << std::endl;


  //passed pawns analysis

  eval += 5 * coefs[ee_passed] * popcount(white_passed & rank_7);
  eval += 4 * coefs[ee_passed] * popcount(white_passed & rank_6);
  eval += 3 * coefs[ee_passed] * popcount(white_passed & rank_5);
  eval += 2 * coefs[ee_passed] * popcount(white_passed & rank_4);
  eval += coefs[ee_passed] * popcount(white_passed & rank_3);


  eval -= 5 * coefs[ee_passed] * popcount(black_passed & rank_2);
  eval -= 4 * coefs[ee_passed] * popcount(black_passed & rank_3);
  eval -= 3 * coefs[ee_passed] * popcount(black_passed & rank_4);
  eval -= 2 * coefs[ee_passed] * popcount(black_passed & rank_5);
  eval -= coefs[ee_passed] * popcount(black_passed & rank_6);

  //std::cout << "passed pawn " << eval << std::endl;


//protected passed pawn
  eval += coefs[ee_passed] * popcount(white_passed & bitboards[white + pawn] & attack_table[white + pawn]);
  eval -= coefs[ee_passed] * popcount(black_passed & bitboards[black + pawn] & attack_table[white + pawn]);

  //rook behind
  bitboard behind_passed = south_line(white_passed) | north_line(black_passed);
  eval += coefs[ee_rook_passed] * popcount(bitboards[white + rook] & behind_passed);
  eval -= coefs[ee_rook_passed] * popcount(bitboards[black + rook] & behind_passed);

  //std::cout << "rook & protect " << eval << std::endl;


  //protection on the way to promotion
  bitboard passed_pass = north_line(white_passed & rank_7);
  int count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    if (count > -1){
      eval += 8 * coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_2);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > -1){
      eval -= 8 * coefs[ee_passed_pawn_block];
    }
  }


  passed_pass = north_line(white_passed & rank_6);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 0){
      eval += 3 * coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_3);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 0){
      eval -= 3 * coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = north_line(white_passed & rank_5);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 1){
      eval += 2 * coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_4);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 1){
      eval -= 2 * coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = north_line(white_passed & rank_4);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 2){
      eval += coefs[ee_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_5);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 2){
      eval -= coefs[ee_passed_pawn_block];
    }
  }

  //std::cout << "pass " << eval << std::endl;




  return eval;
}














int chessboard::eval_endgame(std::array<int, 25> const& coefs){
  int eval = ST_eval + coefs[e_turn];
  //std::cout << "squares tables " << eval << std::endl;
  get_att_table(true);
  get_att_table(false);


  attack_table[white + knight] &= ~attack_table[black + pawn];//squares defended by pawns aren't counted in mobility
  attack_table[white + bishop] &= ~attack_table[black + pawn];
  attack_table[black + knight] &= ~attack_table[white + pawn];
  attack_table[black + bishop] &= ~attack_table[white + pawn];


  //pieces board control
  eval += coefs[e_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & ext_center);
  eval += coefs[e_piece_control] * popcount((attack_table[white + knight] | attack_table[white + bishop]) & black_camp);


  eval -= coefs[e_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & ext_center);
  eval -= coefs[e_piece_control] * popcount((attack_table[black + knight] | attack_table[black + bishop]) & white_camp);

  eval += coefs[e_rook_control] * popcount(attack_table[white + rook] & (ext_center | black_camp));
  eval -= coefs[e_rook_control] * popcount(attack_table[black + rook] & (ext_center | white_camp));

  if(popcount(bitboards[white + bishop]) == 2){ eval += coefs[e_bishop_pair];}
  if(popcount(bitboards[black + bishop]) == 2){ eval -= coefs[e_bishop_pair];}


  bitboard white_outpost = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn])));
  bitboard black_outpost = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn])));
  bitboard white_weak_pawn = ~(W(bitboards[white + pawn]) | E(bitboards[white + pawn])) & bitboards[white + pawn] & black_outpost & S(attack_table[black + pawn] | bitboards[all]);
  bitboard black_weak_pawn = ~(W(bitboards[black + pawn]) | E(bitboards[black + pawn])) &  bitboards[black + pawn] & white_outpost & N(attack_table[white + pawn] | bitboards[all]);
  white_outpost &= N(ext_center);
  black_outpost &= S(ext_center);
  white_outpost &= attack_table[white + pawn];
  black_outpost &= attack_table[black + pawn];
  bitboard white_passed = ~(south_line(W(bitboards[black + pawn]) | E(bitboards[black + pawn]) | bitboards[black + pawn]));
  bitboard black_passed = ~(north_line(W(bitboards[white + pawn]) | E(bitboards[white + pawn]) | bitboards[white + pawn]));
  white_passed &= bitboards[white + pawn];
  black_passed &= bitboards[black + pawn];

  eval += coefs[e_outpost] * popcount(white_outpost & (bitboards[white + knight] | bitboards[white + bishop]));
  eval -= coefs[e_outpost] * popcount(black_outpost & (bitboards[black + knight] | bitboards[black + bishop]));

  //std::cout << "minor pieces " << eval << std::endl;


  bitboard white_open = bitboards[white + pawn];
  white_open |= N(white_open);
  white_open |= N(N(white_open));
  white_open |= N(N(N(white_open)));
  white_open |= S(white_open);
  white_open |= S(S(white_open));
  white_open |= S(S(S(white_open)));
  white_open = ~white_open;

  bitboard black_open = bitboards[black + pawn];
  black_open |= N(black_open);
  black_open |= N(N(black_open));
  black_open |= N(N(N(black_open)));
  black_open |= S(black_open);
  black_open |= S(S(black_open));
  black_open |= S(S(S(black_open)));
  black_open = ~black_open;

  //rooks evaluation

  if (N_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){//doubled rooks
    eval += coefs[e_doubled_rooks];
    if (bitboards[white + rook] & white_open){
      eval += coefs[e_doubled_rooks];
      if (bitboards[white + rook] & black_open){
        eval += 2 * coefs[e_doubled_rooks];
      }
    }
  }

  if (N_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){//doubled rooks
    eval -= coefs[e_doubled_rooks];
    if (bitboards[black + rook] & black_open){
      eval -= coefs[e_doubled_rooks];
      if (bitboards[black + rook] & white_open){
        eval -= 2 * coefs[e_doubled_rooks];
      }
    }
  }

  if (E_attacks(bitboards[white + rook], bitboards[all]) & bitboards[white + rook]){ eval += coefs[e_connected_rooks];}
  if (E_attacks(bitboards[black + rook], bitboards[all]) & bitboards[black + rook]){ eval -= coefs[e_connected_rooks];}



  //rooks on semi open file
  eval += coefs[e_open_rook] * popcount(bitboards[white + rook] & white_open);
  eval -= coefs[e_open_rook] * popcount(bitboards[black + rook] & black_open);

  //rook on open file

  eval += coefs[e_open_rook] * popcount(bitboards[white + rook] & white_open & black_open);
  eval -= coefs[e_open_rook] * popcount(bitboards[black + rook] & white_open & black_open);

  //std::cout << "rooks " << eval << std::endl;

  //pawn_structure
  eval -= coefs[e_weak_pawn] * popcount(white_weak_pawn);
  eval += coefs[e_weak_pawn] * popcount(black_weak_pawn);
  eval += coefs[e_pawn_co] * popcount((bitboards[white + pawn] & ~rank_2) & (E(bitboards[white + pawn]) | W(bitboards[white + pawn]) | S_E(bitboards[white + pawn]) | S_W(bitboards[white + pawn])));
  eval -= coefs[e_pawn_co] * popcount((bitboards[black + pawn] & ~rank_7) & (E(bitboards[black + pawn]) | W(bitboards[black + pawn]) | S_E(bitboards[black + pawn]) | S_W(bitboards[black + pawn])));

  eval -= coefs[e_doubled] * popcount(bitboards[white + pawn] & south_line(bitboards[white + pawn]));
  eval += coefs[e_doubled] * popcount(bitboards[black + pawn] & north_line(bitboards[black + pawn]));

  eval -= coefs[e_blocked] * popcount(bitboards[all] & N(bitboards[white + pawn] & rank_2 & (c_file | d_file | e_file | f_file)));

  eval += coefs[e_blocked] * popcount(bitboards[all] & S(bitboards[black + pawn] & rank_7 & (c_file | d_file | e_file | f_file)));

  bitboard iso_white = bitboards[white + pawn];
  iso_white |= (north_line(iso_white) | south_line(iso_white));
  iso_white = ~(E(iso_white) | W(iso_white));
  iso_white = bitboards[white + pawn] & iso_white;
  eval -= coefs[e_isolated] * popcount(iso_white);

  bitboard iso_black = bitboards[black + pawn];
  iso_black |= (north_line(iso_black) | south_line(iso_black));
  iso_black = ~(E(iso_black) | W(iso_black));
  iso_black = bitboards[black + pawn] & iso_black;
  eval += coefs[e_isolated] * popcount(iso_black);

  //std::cout << "pawn structure " << eval << std::endl;

  eval += coefs[e_pawn_knight] * (popcount(bitboards[white + knight]) - popcount(bitboards[black + knight])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval += coefs[e_pawn_rook] * (popcount(bitboards[white + rook]) - popcount(bitboards[black + rook])) * popcount(bitboards[white + pawn] | bitboards[black + pawn]) / 2;

  eval -= coefs[e_undefended] * popcount((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white]);
  eval += coefs[e_undefended] * popcount((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black]);

  eval += coefs[e_bishop_knight] * popcount( bitboards[white + bishop] & ( S(S(S(bitboards[black + knight]))) | N(N(N(bitboards[black + knight]))) | E(E(E(bitboards[black + knight]))) | W(W(W(bitboards[black + knight])))));
  eval -= coefs[e_bishop_knight] * popcount( bitboards[black + bishop] & ( S(S(S(bitboards[white + knight]))) | N(N(N(bitboards[white + knight]))) | E(E(E(bitboards[white + knight]))) | W(W(W(bitboards[white + knight])))));

  //std::cout << "piece interaction " << eval << std::endl;

  //attack potential evaluation
  bitboard targets = (bitboards[black + bishop] & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[e_knight_att] * popcount(attack_table[white + knight] & targets);

  targets = (bitboards[black + knight] & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[e_bishop_att] * popcount(attack_table[white + bishop] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop]) & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[e_rook_att] * popcount(attack_table[rook + white] & targets);

  targets = ((bitboards[black + knight] | bitboards[black + bishop] | bitboards[black + rook]) & ~attack_table[black + pawn]) | black_weak_pawn | iso_black;
  eval += coefs[e_queen_att] * popcount(attack_table[queen + white] & targets);

  eval += coefs[e_king_att] * popcount(attack_table[white + king] & targets);


  targets = (bitboards[white + bishop] & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_knight_att] * popcount(attack_table[black + knight] & targets);

  targets = (bitboards[white + knight] & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_bishop_att] * popcount(attack_table[black + bishop] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop]) & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_rook_att] * popcount(attack_table[black + rook] & targets);

  targets = ((bitboards[white + knight] | bitboards[white + bishop] | bitboards[white + rook]) & ~attack_table[white + pawn]) | white_weak_pawn | iso_white;
  eval -= coefs[m_queen_att] * popcount(attack_table[queen + black] & targets);

  eval -= coefs[e_king_att] * popcount(attack_table[black + king] & targets);

  //std::cout << "targets " << eval << std::endl;

  //passed pawns analysis


  eval += 5 * coefs[e_passed] * popcount(white_passed & rank_7);
  eval += 4 * coefs[e_passed] * popcount(white_passed & rank_6);
  eval += 3 * coefs[e_passed] * popcount(white_passed & rank_5);
  eval += 2 * coefs[e_passed] * popcount(white_passed & rank_4);
  eval += coefs[e_passed] * popcount(white_passed & rank_3);


  eval -= 5 * coefs[e_passed] * popcount(black_passed & rank_2);
  eval -= 4 * coefs[e_passed] * popcount(black_passed & rank_3);
  eval -= 3 * coefs[e_passed] * popcount(black_passed & rank_4);
  eval -= 2 * coefs[e_passed] * popcount(black_passed & rank_5);
  eval -= coefs[e_passed] * popcount(black_passed & rank_6);


//protected passed pawn
  eval += 2 * coefs[e_passed] * popcount(white_passed & bitboards[white + pawn] & attack_table[white + pawn]);
  eval -= 2 * coefs[e_passed] * popcount(black_passed & bitboards[black + pawn] & attack_table[white + pawn]);

  //rook behind
  bitboard behind_passed = south_line(white_passed) | north_line(black_passed);
  eval += coefs[e_rook_passed] * popcount(bitboards[white + rook] & behind_passed);
  eval -= coefs[e_rook_passed] * popcount(bitboards[black + rook] & behind_passed);

  //protection on the way to promotion
  bitboard passed_pass = north_line(white_passed & rank_7);
  int count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    if (count > -1){
      eval += 8 * coefs[e_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_2);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > -1){
      eval -= 8 * coefs[e_passed_pawn_block];
    }
  }


  passed_pass = north_line(white_passed & rank_6);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 0){
      eval += 3 * coefs[e_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_3);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 0){
      eval -= 3 * coefs[e_passed_pawn_block];
    }
  }

  passed_pass = north_line(white_passed & rank_5);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 1){
      eval += 2 * coefs[e_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_4);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 1){
      eval -= 2 * coefs[e_passed_pawn_block];
    }
  }

  passed_pass = north_line(white_passed & rank_4);
  count = 0;
  if (passed_pass){
    count += popcount(passed_pass & attack_table[white + knight]);
    count += popcount(passed_pass & attack_table[white + bishop]);
    count += popcount(passed_pass & attack_table[white + rook]);
    count += popcount(passed_pass & attack_table[white + queen]);
    count += popcount(passed_pass & attack_table[white + king]);

    count -= popcount(passed_pass & attack_table[black + knight]);
    count -= popcount(passed_pass & attack_table[black + bishop]);
    count -= popcount(passed_pass & attack_table[black + rook]);
    count -= popcount(passed_pass & attack_table[black + queen]);
    count -= popcount(passed_pass & attack_table[black + king]);
    count -= popcount(passed_pass & bitboards[black]);
    if (count > 2){
      eval += coefs[e_passed_pawn_block];
    }
  }

  passed_pass = south_line(black_passed & rank_5);
  count = 0;
  if (passed_pass){
    count -= popcount(passed_pass & attack_table[white + knight]);
    count -= popcount(passed_pass & attack_table[white + bishop]);
    count -= popcount(passed_pass & attack_table[white + rook]);
    count -= popcount(passed_pass & attack_table[white + queen]);
    count -= popcount(passed_pass & attack_table[white + king]);
    count -= popcount(passed_pass & bitboards[white]);

    count += popcount(passed_pass & attack_table[black + knight]);
    count += popcount(passed_pass & attack_table[black + bishop]);
    count += popcount(passed_pass & attack_table[black + rook]);
    count += popcount(passed_pass & attack_table[black + queen]);
    count += popcount(passed_pass & attack_table[black + king]);
    if (count > 2){
      eval -= coefs[e_passed_pawn_block];
    }
  }
  //std::cout << "passed pawns " << eval << std::endl;

  return eval;
}







int chessboard::get_eval(eval_coefs const& coefs){

  int eval;
  if (phase == opening){ eval =  eval_opening(coefs.open_coefs);}
  if (phase == midgame){ eval =  eval_midgame(coefs.midgame_coefs);}
  if (phase == early_end){ eval =  eval_early_end(coefs.early_end_coefs);}
  if (phase == endgame){ eval =  eval_endgame(coefs.endgame_coefs);}

  //insentive to trade for the side who has material advantage

  int white_material = popcount(bitboards[white + pawn]) + 3 * popcount(bitboards[white + knight] | bitboards[white + bishop]);
  white_material += 5 * popcount(bitboards[white + rook]) + 9 * popcount(bitboards[white + queen]);
  int black_material = popcount(bitboards[black + pawn]) + 3 * popcount(bitboards[black + knight] | bitboards[black + bishop]);
  black_material += 5 * popcount(bitboards[black + rook]) + 9 * popcount(bitboards[black + queen]);


  if (black_material == white_material){ return eval;}
  //insentive to trade pieces for the side with material advantage
  //side with material advantage is better
  if (black_material > white_material && eval < 0){ return eval * (150 - material_sum) / 150;}
  if (black_material < white_material && eval > 0){ return eval * (150 - material_sum) / 150;}

  //side with material is worst

  if (black_material > white_material && eval > 0){ return eval * (90 + material_sum) / 150;}
  if (black_material < white_material && eval < 0){ return eval * (90 + material_sum) / 150;}

  return eval;


}
