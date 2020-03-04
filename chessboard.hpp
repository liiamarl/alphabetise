
#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP


#include "bit_function.hpp"
#include <array>
#include <vector>
#include <string>
#include <bitset>
#include "structs.hpp"

extern std::array<std::array<std::array<int, 64>, 6>, 4> square_tables;
void init_ST();



class chessboard{


public:
  chessboard();//initial position
  chessboard(std::array<int, 64> B, int n_turns, std::bitset<4> cr, bitboard EP);//start from a position

  std::array<bitboard, 15> bitboards;

  std::array<int, 64> board;

  void display(bool white_side);

  void display_all();


  //attack bitboards, stored like pieces bitboards
  std::array<bitboard, 14> attack_table;
  void get_att_table(bool white_);


  void flip_color();


  //general game data
  bitboard EP_rights;
  std::bitset<4> castle_rights;
  gamephase phase;
  int turns;
  bool white_to_play;
  int material_sum;//used to determine gamephase
  std::vector<move> played_moves;
  bool in_check;

  float ST_eval;

  bitboard get_pawn_attack(bitboard const& bit, bool white_);
  bitboard get_knight_attack(bitboard const& bit);
  bitboard get_bishop_attack(bitboard const& bit);
  bitboard get_rook_attack(bitboard const& bit);
  bitboard get_queen_attack(bitboard const& bit);
  bitboard get_king_attack(bitboard const& bit);

  void get_dirgolem();


  //legal move generator without any ordering
  std::vector<move> get_all_moves();
  //staged move generation
  //1 = pawn captures and promotions, 2 = captures, 3 = castle, 4 = quiets
  std::vector<move> stage_1_moves();
  std::vector<move> stage_2_moves();
  std::vector<move> stage_3_moves();
  std::vector<move> stage_4_moves();

  move get_hash_move(bitboard hash_move);

  changes make_move(move const& m);

  void unmake_move(move const& m, changes const& ch);

  int get_eval(eval_coefs const& coefs);

  void check_gamephase();


  int SEE(move m);


  std::vector<bitboard> reached_pos;
  void get_hash();


  std::vector<move> moves_in_order(bitboard hash_move);

  std::vector<move> quiescence_moves();

  std::vector<move> final_moves();




private:

//evaluation & move ordering datas

  std::array<int, 64> ST_values;//piece squares tables values in centipawn


  //move generation sub functions

  bitboard N_attacks(bitboard const& bit, bitboard occup);
  bitboard S_attacks(bitboard const& bit, bitboard occup);
  bitboard E_attacks(bitboard const& bit, bitboard occup);
  bitboard W_attacks(bitboard const& bit, bitboard occup);
  bitboard NE_attacks(bitboard const& bit, bitboard occup);
  bitboard NW_attacks(bitboard const& bit, bitboard occup);
  bitboard SE_attacks(bitboard const& bit, bitboard occup);
  bitboard SW_attacks(bitboard const& bit, bitboard occup);










  //actual move generation


  bitboard hor_block, ver_block, dia_block, ant_block;//pins
  std::array<bitboard, 18> dir_golem;

  //make move subfunctions

  void make_castle(move const& m);
  void make_quiet(move const& m);
  void make_capture(move const& m);
  void make_EP(move const& m);
  void make_promotion(move const& m);
  void make_promo_capture(move const& m);

  void unmake_castle(move const& m);
  void unmake_quiet(move const& m);
  void unmake_capture(move const& m);
  void unmake_EP(move const& m);
  void unmake_promotion(move const& m);
  void unmake_promo_capture(move const& m);


  //sub evaluation function

  int eval_opening(std::array<int, 25> const& coefs);
  int eval_midgame(std::array<int, 25> const& coefs);
  int eval_early_end(std::array<int, 27> const& coefs);
  int eval_endgame(std::array<int, 25> const& coefs);



  std::vector<threat> get_attacks(int const ind, bool white_, bitboard const exept);

  threat consider_x_ray(bitboard const target, bitboard const blocker);


  //zorbit's hash values
  std::array<std::array<bitboard, 64>, 14> hash_values;
  std::array<bitboard, 4> castle_hash;
  bitboard EP_hash;

};
















move get_human_move(chessboard C);

#endif
