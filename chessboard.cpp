#include "bit_function.hpp"
#include "chessboard.hpp"
#include "structs.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>



bitboard von_neumann_rand(){//pseudo random 64bit generator
  bitboard res = rand();
  res <<= 16;
  res |= rand();
  res <<= 16;
  res |= rand();
  res <<= 16;
  res |= rand();
  return res;
}



chessboard::chessboard(){
  //initializing bitboards
  bitboards[black + pawn] = 0b0000000011111111000000000000000000000000000000000000000000000000;
  bitboards[white + pawn] = 0b0000000000000000000000000000000000000000000000001111111100000000;
  bitboards[black + knight] = 0b0100001000000000000000000000000000000000000000000000000000000000;
  bitboards[black + bishop] = 0b0010010000000000000000000000000000000000000000000000000000000000;
  bitboards[white + knight] = 0b0000000000000000000000000000000000000000000000000000000001000010;
  bitboards[white + bishop] = 0b0000000000000000000000000000000000000000000000000000000000100100;
  bitboards[black + rook] = 0b1000000100000000000000000000000000000000000000000000000000000000;
  bitboards[white + rook] = 0b0000000000000000000000000000000000000000000000000000000010000001;
  bitboards[white + queen] = 0b0000000000000000000000000000000000000000000000000000000000010000;
  bitboards[black + queen] = 0b0001000000000000000000000000000000000000000000000000000000000000;
  bitboards[black + king] = 0b0000100000000000000000000000000000000000000000000000000000000000;
  bitboards[white + king] = 0b0000000000000000000000000000000000000000000000000000000000001000;
  bitboards[white] = 0b0000000000000000000000000000000000000000000000001111111111111111;
  bitboards[black] = 0b1111111111111111000000000000000000000000000000000000000000000000;
  bitboards[all] = bitboards[black] | bitboards[white];

  white_to_play = true;

  ST_eval = 0;





  castle_rights = 15;
  EP_rights = 0;
  phase = opening;

  board = {white + rook, white + knight, white + bishop, white + king, white + queen, white + bishop, white + knight, white + rook,
    white + pawn, white + pawn, white + pawn, white + pawn, white + pawn, white + pawn, white + pawn, white + pawn,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    black + pawn, black + pawn, black + pawn, black + pawn, black + pawn, black + pawn, black + pawn, black + pawn,
    black + rook, black + knight, black + bishop, black + king, black + queen, black + bishop, black + knight, black + rook
  };

  turns = 0;
  material_sum = 74;


  for (auto it = ST_values.begin(); it != ST_values.end(); ++it){
    (*it) = 0;
  }
  for (int c = 0; c < 16; c++){
    ST_values[c] = square_tables[phase][board[c] / 2 - 1][c];
  }
  for (int c = 48; c < 64; c++){
    ST_values[c] = -square_tables[phase][board[c] / 2 - 1][flip_mailbox[c]];
  }
  ST_eval = 0;
  for (auto it = ST_values.begin(); it != ST_values.end(); ++it){
    ST_eval += (*it);
  }

  for (int c = 0; c < 14; c++){
    for (int k = 0; k < 64; k++){
      hash_values[c][k] = von_neumann_rand();
    }
  }
  for (int c = 0; c < 4; c++){ castle_hash[c] = von_neumann_rand();}
  EP_hash = von_neumann_rand();
}




void chessboard::get_hash(){//zorbit's hash
  bitboard res = 0;
  for (int c = 0; c < 64; c++){
    if (bitboards[all] & (one << c)){
      res ^= hash_values[board[c]][c];
    }
  }
  for(int c = 0; c < 4; c++){
    if (castle_rights[c]){ res ^= castle_hash[c];}
  }
  if (EP_rights){ res ^= EP_hash;}
  reached_pos.push_back(res);
}








chessboard::chessboard(std::array<int, 64> B, int n_turns, std::bitset<4> cr, bitboard EP){//constructor with specific position on the board
  bitboards = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  std::array<int, 64> assign = {
    63, 62, 61, 60, 59, 58, 57, 56,
    55, 54, 53, 52, 51, 50, 49, 48,
    47, 46, 45, 44, 43, 42, 41, 40,
    39, 38, 37, 36, 35, 34, 33, 32,
    31, 30, 29, 28, 27, 26, 25, 24,
    23, 22, 21, 20, 19, 18, 17, 16,
    15, 14, 13, 12, 11, 10, 9, 8,
    7, 6, 5, 4, 3, 2, 1, 0
  };
  bitboard ref = one << 63;
  for (int c = 0; c < 64; c++){
    if (B[c] != 0){
      bitboards[B[c]] ^= ref;
      board[assign[c]] = B[c];
    }

    ref >>= 1;
  }
  bitboards[black] = bitboards[black + pawn] | bitboards[black + knight] | bitboards[black + bishop] | bitboards[black + rook] | bitboards[black + queen] | bitboards[black + king];
  bitboards[white] = bitboards[white + pawn] | bitboards[white + knight] | bitboards[white + bishop] | bitboards[white + rook] | bitboards[white + queen] | bitboards[white + king];
  bitboards[all] = bitboards[black] | bitboards[white];

  material_sum = 3 * popcount(bitboards[white + knight] | bitboards[black + knight] | bitboards[white + bishop] | bitboards[black + bishop]);
  material_sum += 5 * popcount(bitboards[black + rook] | bitboards[white + rook]);
  material_sum += 15 * popcount(bitboards[white + queen] | bitboards[black + queen]);


  white_to_play = true;

  ST_eval = 0;



  ST_values = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


  castle_rights = cr;
  EP_rights = EP;


  turns = n_turns;

  check_gamephase();


  for (int c = 0; c < 64; c++){
    if ((one << c) & bitboards[black]){
      ST_values[c] = -square_tables[phase][board[c] / 2 - 1][flip_mailbox[c]];

    }
    if ((one << c) & bitboards[white]){
      ST_values[c] = square_tables[phase][board[c] / 2 - 1][c];
    }

  }
  ST_eval = 0;
  for (auto it = ST_values.begin(); it != ST_values.end(); ++it){
    ST_eval += (*it);
  }

  for (int c = 0; c < 14; c++){
    for (int k = 0; k < 64; k++){
      hash_values[c][k] = von_neumann_rand();
    }
  }
  for (int c = 0; c < 4; c++){ castle_hash[c] = von_neumann_rand();}
  EP_hash = von_neumann_rand();

  for (int c = 0; c < turns; c++){
    move m;
    m.type = quiet;
    m.piece = pawn;
    played_moves.push_back(m);
    reached_pos.push_back(c);
  }
}




void chessboard::display(bool white_side){//diaplays the board on the terminal
  if (white_side){
    std::cout << "    ___A____B____C____D____E____F____G____H" << std::endl << std::endl << " 8 | ";
    for (int c = 63; c > -1; c--){
      if (c == 55){ std::cout << std::endl << std::endl << " 7 | ";}
      if (c == 47){ std::cout << std::endl << std::endl << " 6 | ";}
      if (c == 39){ std::cout << std::endl << std::endl << " 5 | ";}
      if (c == 31){ std::cout << std::endl << std::endl << " 4 | ";}
      if (c == 23){ std::cout << std::endl << std::endl << " 3 | ";}
      if (c == 15){ std::cout << std::endl << std::endl << " 2 | ";}
      if (c == 7){ std::cout << std::endl << std::endl << " 1 | ";}
      long long ref = 0b0000000000000000000000000000000000000000000000000000000000000001;
      ref <<= c;
      if (bitboards[pawn + white] & ref){ std::cout << "  P  ";}
      if (bitboards[knight + white] & ref){ std::cout << "  N  ";}
      if (bitboards[bishop + white] & ref){ std::cout << "  B  ";}
      if (bitboards[rook + white] & ref){ std::cout << "  R  ";}
      if (bitboards[queen + white] & ref){ std::cout << "  Q  ";}
      if (bitboards[king + white] & ref){ std::cout << "  K  ";}
      if (bitboards[pawn + black] & ref){ std::cout << " |P| ";}
      if (bitboards[knight + black] & ref){ std::cout << " |N| ";}
      if (bitboards[bishop + black] & ref){ std::cout << " |B| ";}
      if (bitboards[rook + black] & ref){ std::cout << " |R| ";}
      if (bitboards[queen + black] & ref){ std::cout << " |Q| ";}
      if (bitboards[king + black] & ref){ std::cout << " |K| ";}
      if (!(ref & bitboards[all])){ std::cout << "  _  ";}
    }
  }
  else{
    std::cout << "    ___H____G____F____E____D____C____B____A" << std::endl << std::endl << " 1 | ";
    for (int c = 0; c < 64; c++){
      if (c == 48){ std::cout << std::endl << std::endl << " 7 | ";}
      if (c == 40){ std::cout << std::endl << std::endl << " 6 | ";}
      if (c == 32){ std::cout << std::endl << std::endl << " 5 | ";}
      if (c == 24){ std::cout << std::endl << std::endl << " 4 | ";}
      if (c == 16){ std::cout << std::endl << std::endl << " 3 | ";}
      if (c == 8){ std::cout << std::endl << std::endl << " 2 | ";}
      if (c == 56){ std::cout << std::endl << std::endl << " 8 | ";}
      long long ref = 0b0000000000000000000000000000000000000000000000000000000000000001;
      ref <<= c;
      if (bitboards[pawn + white] & ref){ std::cout << "  P  ";}
      if (bitboards[knight + white] & ref){ std::cout << "  N  ";}
      if (bitboards[bishop + white] & ref){ std::cout << "  B  ";}
      if (bitboards[rook + white] & ref){ std::cout << "  R  ";}
      if (bitboards[queen + white] & ref){ std::cout << "  Q  ";}
      if (bitboards[king + white] & ref){ std::cout << "  K  ";}
      if (bitboards[pawn + black] & ref){ std::cout << " |P| ";}
      if (bitboards[knight + black] & ref){ std::cout << " |N| ";}
      if (bitboards[bishop + black] & ref){ std::cout << " |B| ";}
      if (bitboards[rook + black] & ref){ std::cout << " |R| ";}
      if (bitboards[queen + black] & ref){ std::cout << " |Q| ";}
      if (bitboards[king + black] & ref){ std::cout << " |K| ";}
      if (!(ref & bitboards[all])){ std::cout << "  _  ";}
    }
  }
  std::cout << std::endl << std::endl;
}



void chessboard::display_all(){//display many infos about the current state of the chessboard object
  std::cout << "////////////START DISPLAY////////////////////////" << std::endl << std::endl;
  display(true);
  for (int i = 7; i > -1; i--){
    for (int j = 7; j > -1; j--){
      std::cout << " " << board[8 * i + j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "EP rights :" << std::endl;
  display_bitboard(EP_rights);
  std::cout << "castle rights : " << castle_rights << std::endl;
  std::cout << "gamephase : " << phase << std::endl;
  std::cout << turns << " turns" << std::endl;
  std::cout << "material sum : " << material_sum << std::endl;

  std::cout << "square table evaluation :" << std::endl;
  for (int i = 7; i > -1; i--){
    for (int j = 7; j > -1; j--){
      std::cout << " " << ST_values[8 * i + j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "total : " << ST_eval << std::endl;
  std::cout << std::endl << "::::::::::END:::::::::::::::::::::::" << std::endl;
}





void chessboard::check_gamephase(){//update the gamephase based on remaining material
  //gamephases are opening, midgame, early endgame and endgame
  gamephase gp = midgame;
  if (turns < 20){ gp = opening;}
  if (material_sum < 39){ gp = early_end;}
  if (material_sum < 25){ gp = endgame;}
  if (gp != phase){
    phase = gp;
    ST_eval = 0;
    int ind = 64;
    for (bitboard bit = one << 63; bit != 0; bit >>= 1){
      ind--;
      if (bitboards[white] & bit){
        ST_values[ind] = square_tables[phase][board[ind] / 2 - 1][ind];
        ST_eval += ST_values[ind];
      }
      if (bitboards[black] & bit){
        ST_values[ind] = -square_tables[phase][board[ind] / 2 - 1][flip_mailbox[ind]];
        ST_eval += ST_values[ind];
      }
    }
  }

}
