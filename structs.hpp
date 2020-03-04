#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <array>
#include <string>
#include "bit_function.hpp"
#include <bitset>






//just a lot of structures, enumerations and constants used in the engine


const std::array<int, 15> material_value = {
  0, 0, 1, 1, 3, 3, 3, 3, 5, 5, 15, 15, 100, 100, 0
};



enum piece_ind{
  black = 0,
  white = 1,
  pawn = 2,
  knight = 4,
  bishop = 6,
  rook = 8,
  queen = 10,
  king = 12,
  all = 14
};

const std::array<std::string, 15> piece_display = {
  "black", "white", "black pawn", "white pawn", "black knight", "white knight",
  "black_bishop", "white_bishop", "black rook", "white rook", "black queen",
  "white queen", "black king", "white king", "all"
};


const std::array<std::string, 64> square_display = {
  "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
  "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
  "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
  "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
  "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
  "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
  "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
  "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
};

enum gamephase{
  opening,
  midgame,
  early_end,
  endgame
};


extern const std::array<int, 64> flip_mailbox;



enum diretion{
  dir_N,
  dir_S,
  dir_E,
  dir_W,
  dir_NW,
  dir_NE,
  dir_SE,
  dir_SW,
  dir_SSE,
  dir_SSW,
  dir_SEE,
  dir_SWW,
  dir_NNE,
  dir_NNW,
  dir_NWW,
  dir_NEE,
  dir_castle,
  dir_EP
};

enum move_type{
  quiet,
  capture,
  en_passant,
  castle,
  promotion,
  promo_capture,
};



struct move{
  int from, to;
  bitboard from_b, to_b;
  int piece;
  int target;
  int type;
  int eval;
  int search_depth;
  int depth_reduction;
};


void display_move(move m);


struct changes{
  std::bitset<4> castle;
  bitboard EP;
  std::array<int, 64> ST, board;
  int ev;
  std::array<bitboard, 18> golem;
  bool check;
  gamephase gp;
};


struct threat{
  int ind;
  int ST_value;
  bitboard bit;
};


struct pos_info{
  bitboard full_hash;
  int eval;
  bitboard best_move;
  int search_depth;
  int base_turn;
};


enum eval_open_coef{
  o_turn,
  o_piece_control,
  o_bishop_pair,
  o_outpost,
  o_passed,
  o_shield,
  o_tropism,
  o_storm,
  o_doubled_rooks,
  o_connected_rooks,
  o_open_rook,
  o_weak_pawn,
  o_pawn_co,
  o_doubled,
  o_blocked,
  o_isolated,
  o_pawn_knight,
  o_pawn_rook,
  o_queen_rook,
  o_bishop_knight,
  o_undefended,
  o_knight_att,
  o_bishop_att,
  o_rook_att,
  o_queen_att,

};


enum eval_mid_coef{
  m_turn,
  m_piece_control,
  m_bishop_pair,
  m_outpost,
  m_passed,
  m_shield,
  m_tropism,
  m_storm,
  m_doubled_rooks,
  m_connected_rooks,
  m_open_rook,
  m_weak_pawn,
  m_pawn_co,
  m_doubled,
  m_blocked,
  m_isolated,
  m_pawn_knight,
  m_pawn_rook,
  m_queen_rook,
  m_bishop_knight,
  m_undefended,
  m_knight_att,
  m_bishop_att,
  m_rook_att,
  m_queen_att,
};



enum eval_early_coef{
  ee_turn,
  ee_piece_control,
  ee_rook_control,
  ee_bishop_pair,
  ee_outpost,
  ee_passed,
  ee_shield,
  ee_tropism,
  ee_doubled_rooks,
  ee_connected_rooks,
  ee_open_rook,
  ee_weak_pawn,
  ee_pawn_co,
  ee_doubled,
  ee_blocked,
  ee_isolated,
  ee_pawn_knight,
  ee_pawn_rook,
  ee_undefended,
  ee_bishop_knight,
  ee_knight_att,
  ee_bishop_att,
  ee_rook_att,
  ee_queen_att,
  ee_king_att,
  ee_rook_passed,
  ee_passed_pawn_block,
};

enum eval_end_coef{
  e_turn,
  e_piece_control,
  e_rook_control,
  e_bishop_pair,
  e_outpost,
  e_passed,
  e_doubled_rooks,
  e_connected_rooks,
  e_open_rook,
  e_weak_pawn,
  e_pawn_co,
  e_doubled,
  e_blocked,
  e_isolated,
  e_pawn_knight,
  e_pawn_rook,
  e_undefended,
  e_bishop_knight,
  e_knight_att,
  e_bishop_att,
  e_rook_att,
  e_queen_att,
  e_king_att,
  e_rook_passed,
  e_passed_pawn_block
};

struct eval_coefs{
  std::array<int, 25> open_coefs;
  std::array<int, 25> midgame_coefs;
  std::array<int, 27> early_end_coefs;
  std::array<int, 25> endgame_coefs;
};






#endif
