#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "chessboard.hpp"
#include "structs.hpp"
#include "bit_function.hpp"
#include <array>
#include <iostream>






class database{
public:
  database();
  std::array<std::array<int, 64>, 64> history;
  pos_info get_info(bitboard hash);
  void give_info(pos_info p);
  void display_stats();
  void reset_stats();


private:
  std::array<pos_info, 32768> data;
  int n_acces;
  int n_find;

};




























#endif
