#include "chessboard.hpp"
#include "structs.hpp"
#include "bit_function.hpp"
#include <iostream>
#include <vector>
#include <array>

const std::array<int, 15> switch_pieces = {
  white, black, pawn + white, pawn + black, knight + white, knight + black,
  white + bishop, black + bishop, white + rook, black + rook, white + queen,
  black + queen, white + king, black + king, all};

//this engine is based on color flipping, witch mean that the blacks pieces never actually moves,
//the board and color of the pieces are flipped after each moves and only the white pieces moves
//this is what the following function does
void chessboard::flip_color(){
  bitboard temp;
  for (int c = 0; c < 14; c+= 2){
    temp = flip(bitboards[c]);
    bitboards[c] = flip(bitboards[c + 1]);
    bitboards[c + 1] = temp;
  }
  bitboards[14] = flip(bitboards[14]);
  EP_rights = flip(EP_rights);
  std::bitset<4> cr;
  cr[0] = castle_rights[2]; cr[1] = castle_rights[3];
  cr[2] = castle_rights[0]; cr[3] = castle_rights[1];
  castle_rights = cr;
  for (int c = 0; c < 32; c++){
    int temp_ = -ST_values[c];
    ST_values[c] = -ST_values[flip_mailbox[c]];
    ST_values[flip_mailbox[c]] = temp_;
    temp = board[c];
    board[c] = board[flip_mailbox[c]];
    board[flip_mailbox[c]] = temp;
  }
  for (auto it = board.begin(); it != board.end(); ++it){
    if (*it != 0){
      (*it) = switch_pieces[*it];
    }

  }
  ST_eval *= -1;
  in_check = false;
};



//make move function, 6 functions for 6 types of moves :
//quiet moves, captures, castle, en passant, promotion and promotion with capture

void chessboard::make_castle(move const& m){
  if (m.to == 1){
    bitboards[white + king] = 2;
    bitboards[white + rook] ^= 0b101;
    bitboards[white] ^= 0b1111;
    bitboards[all] ^= 0b1111;
    board[0] = 0;
    board[1] = white + king;
    board[2] = white + rook;
    board[3] = 0;
    castle_rights[0] = 0; castle_rights[1] = 0;
    ST_eval -= (ST_values[3] + ST_values[0]);
    ST_values[1] = square_tables[phase][5][1];
    ST_values[2] = square_tables[phase][3][2];
    ST_values[0] = 0;
    ST_values[3] = 0;
    ST_eval += ST_values[1] + ST_values[2];
  }
  if (m.to == 5){
    bitboards[white + king] = 0b100000;
    bitboards[white + rook] ^= 0b10010000;
    bitboards[white] ^= 0b10111000;
    bitboards[all] ^= 0b10111000;
    board[7] = 0;
    board[5] = white + king;
    board[4] = white + rook;
    board[3] = 0;
    castle_rights[0] = 0; castle_rights[1] = 0;
    ST_eval -= (ST_values[3] + ST_values[7]);
    ST_values[5] = square_tables[phase][5][1];
    ST_values[4] = square_tables[phase][3][2];
    ST_values[7] = 0;
    ST_values[3] = 0;
    ST_eval += ST_values[4] + ST_values[5];
  }
}


void chessboard::make_quiet(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b | m.to_b;
  board[m.from] = 0;
  board[m.to] = m.piece;
  if (castle_rights[0]){
    if ((m.from_b) & 0b1001){ castle_rights[0] = 0;}
  }
  if (castle_rights[1]){
    if ((m.from_b) & 0b10001000){ castle_rights[1] = 0;}
  }
  if (m.piece == white + pawn){
    if (m.from_b & rank_2 && m.to_b & rank_4){ EP_rights = S(m.to_b);}
  }
  ST_eval -= ST_values[m.from];
  ST_values[m.from] = 0;
  ST_values[m.to] = square_tables[phase][m.piece / 2 - 1][m.to];
  ST_eval += ST_values[m.to];
}


void chessboard::make_capture(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[m.target] ^= m.to_b;
  bitboards[black] ^= m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b;
  board[m.from] = 0;
  board[m.to] = m.piece;
  if (castle_rights[0]){
    if ((m.from_b) & 0b1001){ castle_rights[0] = 0;}
  }
  if (castle_rights[1]){
    if ((m.from_b) & 0b10001000){ castle_rights[1] = 0;}
  }
  if (castle_rights[2]){
    if (m.to_b & ((one << 56) | (one << 59))){ castle_rights[2] = 0;}
  }
  if (castle_rights[3]){
    if (m.to_b & ((one << 63) | (one << 59))){ castle_rights[3] = 0;}
  }
  ST_eval -= ST_values[m.from];
  ST_eval -= ST_values[m.to];
  ST_values[m.from] = 0;
  ST_values[m.to] = square_tables[phase][m.piece / 2 - 1][m.to];
  ST_eval += ST_values[m.to];
  if (material_value[m.target] > 1){ material_sum -= material_value[m.target];}
}


void chessboard::make_EP(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b | m.to_b | S(m.to_b);
  bitboards[black] ^= S(m.to_b);
  bitboards[black + pawn] ^= S(m.to_b);
  board[m.from] = 0;
  board[m.to] = white + pawn;
  board[m.to - 8] = 0;
  ST_eval -= (ST_values[m.from] + ST_values[m.to - 8]);
  ST_values[m.to - 8] = 0;
  ST_values[m.from] = 0;
  ST_values[m.to] = square_tables[phase][0][m.to];
  ST_eval += ST_values[m.to];
}


void chessboard::make_promotion(move const& m){
  bitboards[white + pawn] ^= m.from_b;
  bitboards[white + queen] ^= m.to_b;
  bitboards[white] ^= m.to_b | m.from_b;
  bitboards[all] ^= m.to_b | m.from_b;
  board[m.from] = 0;
  board[m.to] = white + queen;
  ST_eval -= ST_values[m.from];
  ST_values[m.from] = 0;
  ST_values[m.to] = square_tables[phase][4][m.to];
  ST_eval += ST_values[m.to];
  material_sum += 9;
}


void chessboard::make_promo_capture(move const& m){
  bitboards[white + pawn] ^= m.from_b;
  bitboards[white + queen] ^= m.to_b;
  bitboards[white] ^= m.to_b | m.from_b;
  bitboards[all] ^= m.from_b;
  bitboards[m.target] ^= m.to_b;
  bitboards[black] ^= m.to_b;
  board[m.from] = 0;
  board[m.to] = white + queen;
  if (castle_rights[2]){
    if (m.to_b & ((one << 56) | (one << 59))){ castle_rights[2] = 0;}
  }
  if (castle_rights[3]){
    if (m.to_b & ((one << 63) | (one << 59))){ castle_rights[3] = 0;}
  }
  ST_eval -= (ST_values[m.from] + ST_values[m.to]);
  ST_values[m.from] = 0;
  ST_values[m.to] = square_tables[phase][4][m.to];
  ST_eval += ST_values[m.to];
  if (material_value[m.target] > 1){ material_sum -= material_value[m.target];}
  material_sum += 9;
}




//main make move function witch calls one of the other six depending on the move type
changes chessboard::make_move(move const& m){
  changes ch;
  ch.castle = castle_rights;
  ch.EP = EP_rights;
  ch.ST = ST_values;
  ch.ev = ST_eval;
  ch.board = board;
  ch.golem = dir_golem;
  ch.check = in_check;
  ch.gp = phase;
  EP_rights = 0;


  if (m.type == castle){ make_castle(m);}
  else if (m.type == quiet){ make_quiet(m);}
  else if (m.type == capture){ make_capture(m);}
  else if (m.type == en_passant){ make_EP(m);}
  else if (m.type == promotion){ make_promotion(m);}
  else if (m.type == promo_capture){ make_promo_capture(m);}
  white_to_play = !white_to_play;
  turns++;
  played_moves.push_back(m);
  get_hash();
  check_gamephase();
  flip_color();
  return ch;
}


//unmake move functions witch undo what the previous ones did



void chessboard::unmake_castle(move const& m){
  if (m.to == 1){
    bitboards[white + king] = 8;
    bitboards[white + rook] ^= 0b101;
    bitboards[white] ^= 0b1111;
    bitboards[all] ^= 0b1111;
  }
  if (m.to == 5){
    bitboards[white + king] = 8;
    bitboards[white + rook] ^= 0b10010000;
    bitboards[white] ^= 0b10111000;
    bitboards[all] ^= 0b10111000;
  }
}


void chessboard::unmake_quiet(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b | m.to_b;
}


void chessboard::unmake_capture(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[m.target] ^= m.to_b;
  bitboards[black] ^= m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b;
  if (material_value[m.target] > 1){ material_sum += material_value[m.target];}
}

void chessboard::unmake_EP(move const& m){
  bitboards[m.piece] ^= m.from_b | m.to_b;
  bitboards[white] ^= m.from_b | m.to_b;
  bitboards[all] ^= m.from_b | m.to_b | S(m.to_b);
  bitboards[black] ^= S(m.to_b);
  bitboards[black + pawn] ^= S(m.to_b);
}

void chessboard::unmake_promotion(move const& m){
  bitboards[white + pawn] ^= m.from_b;
  bitboards[white + queen] ^= m.to_b;
  bitboards[white] ^= m.to_b | m.from_b;
  bitboards[all] ^= m.to_b | m.from_b;
  material_sum -= 9;
}

void chessboard::unmake_promo_capture(move const& m){
  bitboards[white + pawn] ^= m.from_b;
  bitboards[white + queen] ^= m.to_b;
  bitboards[white] ^= m.to_b | m.from_b;
  bitboards[all] ^= m.from_b;
  bitboards[m.target] ^= m.to_b;
  bitboards[black] ^= m.to_b;
  if (material_value[m.target] > 1){ material_sum += material_value[m.target];}
  material_sum -= 9;
}




void chessboard::unmake_move(move const& m, changes const& ch){
  flip_color();
  turns--;
  white_to_play = !white_to_play;
  played_moves.erase(played_moves.end() - 1);
  reached_pos.erase(reached_pos.end() - 1);
  if (m.type == castle){ unmake_castle(m);}
  else if (m.type == quiet){ unmake_quiet(m);}
  else if (m.type == capture){ unmake_capture(m);}
  else if (m.type == en_passant){ unmake_EP(m);}
  else if (m.type == promotion){ unmake_promotion(m);}
  else if (m.type == promo_capture){ unmake_promo_capture(m);}

  castle_rights = ch.castle;
  ST_values = ch.ST;
  board = ch.board;
  ST_eval = ch.ev;
  EP_rights = ch.EP;
  dir_golem = ch.golem;
  in_check = ch.check;
  phase = ch.gp;
}
