#include "bit_function.hpp"
#include <iostream>



//lot of bitboards constants, used mostly in evaluation

const bitboard a_file = 0b1000000010000000100000001000000010000000100000001000000010000000;
const bitboard b_file = 0b0100000001000000010000000100000001000000010000000100000001000000;
const bitboard c_file = 0b0010000000100000001000000010000000100000001000000010000000100000;
const bitboard d_file = 0b0001000000010000000100000001000000010000000100000001000000010000;
const bitboard e_file = 0b0000100000001000000010000000100000001000000010000000100000001000;
const bitboard f_file = 0b0000010000000100000001000000010000000100000001000000010000000100;
const bitboard g_file = 0b0000001000000010000000100000001000000010000000100000001000000010;
const bitboard h_file = 0b0000000100000001000000010000000100000001000000010000000100000001;


const bitboard rank_1 = 0b0000000000000000000000000000000000000000000000000000000011111111;
const bitboard rank_2 = 0b0000000000000000000000000000000000000000000000001111111100000000;
const bitboard rank_3 = 0b0000000000000000000000000000000000000000111111110000000000000000;
const bitboard rank_4 = 0b0000000000000000000000000000000011111111000000000000000000000000;
const bitboard rank_5 = 0b0000000000000000000000001111111100000000000000000000000000000000;
const bitboard rank_6 = 0b0000000000000000111111110000000000000000000000000000000000000000;
const bitboard rank_7 = 0b0000000011111111000000000000000000000000000000000000000000000000;
const bitboard rank_8 = 0b1111111100000000000000000000000000000000000000000000000000000000;

const bitboard center = 0b0000000000000000000000000001100000011000000000000000000000000000;

const bitboard ext_center = 0b0000000000000000001111000011110000111100001111000000000000000000;

const bitboard one = 1;

const bitboard white_camp = 0b01111110011111101111111111111111;
const bitboard black_camp = 0b1111111111111111011111100111111000000000000000000000000000000000;





void display_bitboard(bitboard bit){
  bitboard mask = 0b1000000000000000000000000000000000000000000000000000000000000000;
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      if (bit & mask){ std::cout << "1 ";}
      else{ std::cout << "0 ";}
      mask >>= 1;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}


//following functions returns lines starting from all ones in the input bitboard
//mostly used for sliding pieces move generation and evaluation

bitboard south_line(bitboard const& bit){
  bitboard res = (bit & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  res |= (res & ~rank_1) >> 8;
  return res;
}

bitboard north_line(bitboard const& bit){
  bitboard res = (bit & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  res |= (res & ~rank_8) << 8;
  return res;
}

bitboard east_line(bitboard const& bit){
  bitboard res = (bit & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  res |= (res & ~h_file) >> 1;
  return res;
}


bitboard west_line(bitboard const& bit){
  bitboard res = (bit & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  res |= (res & ~a_file) << 1;
  return res;
}

bitboard south_east_line(bitboard const& bit){
  bitboard res = S_E(bit);
  res |= S_E(bit);
  res |= S_E(bit);
  res |= S_E(bit);
  res |= S_E(bit);
  res |= S_E(bit);
  res |= S_E(bit);
  return res;
}
bitboard north_east_line(bitboard const& bit){
  bitboard res = N_E(bit);
  res |= N_E(bit);
  res |= N_E(bit);
  res |= N_E(bit);
  res |= N_E(bit);
  res |= N_E(bit);
  res |= N_E(bit);
  return res;
}
bitboard south_west_line(bitboard const& bit){
  bitboard res = S_W(bit);
  res |= S_W(bit);
  res |= S_W(bit);
  res |= S_W(bit);
  res |= S_W(bit);
  res |= S_W(bit);
  res |= S_W(bit);
  return res;
}
bitboard north_west_line(bitboard const& bit){
  bitboard res = N_W(bit);
  res |= N_W(bit);
  res |= N_W(bit);
  res |= N_W(bit);
  res |= N_W(bit);
  res |= N_W(bit);
  res |= N_W(bit);
  return res;
}


bitboard const pop_1 = 0x5555555555555555;
bitboard const pop_2 = 0x3333333333333333;
bitboard const pop_3 = 0x0f0f0f0f0f0f0f0f;
bitboard const pop_4 = 0x0101010101010101;


int popcount(bitboard x){//no idea how it works, but it does
  //returns the number of ones in the input bitboard
  x = x - ((x >> 1) & pop_1);
  x = (x & pop_2) + ((x >> 2) & pop_2);
  x = (x + (x >> 4)) & pop_3;
  x = (x * pop_4) >> 56;
  return int(x);
}
