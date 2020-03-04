#include "bit_function.hpp"
#include <iostream>
#include "structs.hpp"
#include "chessboard.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bitset>



//static exchange evaluation
//first functions are used in the main one



//checks if a sliding piece might capture the target square after another piece did
threat chessboard::consider_x_ray(bitboard const target, bitboard const blocker){
  if (blocker & N_attacks(target, bitboards[all])){
    bitboard temp = N_attacks(blocker, bitboards[all]) & (bitboards[white + rook] |bitboards[black + rook] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & S_attacks(target, bitboards[all])){
    bitboard temp = S_attacks(blocker, bitboards[all]) & (bitboards[white + rook] |bitboards[black + rook] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & E_attacks(target, bitboards[all])){
    bitboard temp = E_attacks(blocker, bitboards[all]) & (bitboards[white + rook] |bitboards[black + rook] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & W_attacks(target, bitboards[all])){
    bitboard temp = W_attacks(blocker, bitboards[all]) & (bitboards[white + rook] |bitboards[black + rook] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & SW_attacks(target, bitboards[all])){
    bitboard temp = SW_attacks(blocker, bitboards[all]) & (bitboards[white + bishop] |bitboards[black + bishop] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & SE_attacks(target, bitboards[all])){
    bitboard temp = SE_attacks(blocker, bitboards[all]) & (bitboards[white + bishop] |bitboards[black + bishop] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & NE_attacks(target, bitboards[all])){
    bitboard temp = NE_attacks(blocker, bitboards[all]) & (bitboards[white + bishop] |bitboards[black + bishop] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  else if (blocker & NW_attacks(target, bitboards[all])){
    bitboard temp = NW_attacks(blocker, bitboards[all]) & (bitboards[white + bishop] |bitboards[black + bishop] |bitboards[white + queen] |bitboards[black + queen]);
    if (temp){
      threat th;
      th.bit = temp;
      th.ind = std::log2(temp);
      th.ST_value = ST_values[th.ind];
      return th;
    }
  }
  threat th;
  th.ind = -1;
  return th;
}



bool sort_threats_1(threat const& th1, threat const& th2){ return th1.ST_value < th2.ST_value;}
bool sort_threats_2(threat const& th1, threat const& th2){ return th1.ST_value > th2.ST_value;}


//returns a list of all pieces that attack or deffend a given square
std::vector<threat> chessboard::get_attacks(int const ind, bool white_, bitboard const exept){
  std::vector<threat> threats;
  bitboard bit = one << ind;
  bitboard temp = get_pawn_attack(bit, !white_) & bitboards[pawn + white_];
  temp |= get_knight_attack(bit) & bitboards[knight + white_];
  temp |= get_bishop_attack(bit) & (bitboards[bishop + white_] | bitboards[queen + white_]);
  temp |= get_rook_attack(bit) & (bitboards[rook + white_] | bitboards[queen + white_]);
  temp |= get_king_attack(bit) & bitboards[king + white_];
  temp &= ~exept;
  while(temp){
    threat th;
    th.ind = std::log2(temp);
    th.ST_value = ST_values[th.ind];
    th.bit = one << th.ind;
    threats.push_back(th);
    temp ^= th.bit;
  }
  if(white_){
    std::sort(threats.begin(), threats.end(), sort_threats_1);
  }
  else{
    std::sort(threats.begin(), threats.end(), sort_threats_2);
  }

  return threats;
};




int max_(int const& i1, int const& i2){
  if (i1 > i2){ return i1;}
  return i2;
}





//actual static exchange evaluation, extended to quiet moves and promotions
int chessboard::SEE(move m){
  std::vector<threat> attack;
  threat th;
  th.ind = m.from;
  th.bit = m.from_b;
  th.ST_value = ST_values[m.from];
  attack.push_back(th);
  std::vector<threat> at = get_attacks(m.to, true, m.from_b);
  attack.insert(attack.end(), at.begin(), at.end());
  std::vector<threat> defense = get_attacks(m.to, false, 0);

  bitboard may_x_ray = bitboards[white + pawn] | bitboards[black + pawn] | bitboards[white + bishop] | bitboards[black + bishop] | bitboards[white + rook] | bitboards[black + rook] | bitboards[white + queen] | bitboards[black + queen];
  if (defense.size() == 0){
    if (m.type == promotion){ return 800;}
    if (m.type == promo_capture){ return 800 - ST_values[m.to];}
    return -ST_values[m.to];
  }

  std::bitset<4> x_rays = 0;
  int d = 0;
  std::array<int, 20> gain = {-ST_values[m.to], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if (m.type == promotion || m.type == promo_capture){ gain[0] += 8;}
  while (true){
    d++;
    if ((d & 1) && defense.size() == 0){ break;}
    if (!(d & 1) && attack.size() == 0){ break;}
    if (d & 1){
      gain[d] = gain[d - 1] - attack[0].ST_value;
      if (defense[0].bit & may_x_ray){
        threat th = consider_x_ray(m.to_b, defense[0].bit);

        if (th.ind != -1){
          if (bitboards[black] & th.bit){
            defense.push_back(th);
            x_rays[2] = 1;
          }
          if (bitboards[white] & th.bit){
            attack.push_back(th);
            x_rays[1] = 1;
          }
        }
      }
      if (d == 1){
        if (attack[0].bit & may_x_ray){
          threat th = consider_x_ray(m.to_b, attack[0].bit);
          if (th.ind != -1){
            if (bitboards[black] & th.bit){
              defense.push_back(th);
              x_rays[2] = 1;
            }
            if (bitboards[white] & th.bit){
              attack.push_back(th);
              x_rays[1] = 1;
            }
          }
        }
      }
      attack.erase(attack.begin());
    }
    else{
      gain[d] = gain[d - 1] - defense[0].ST_value;
      if (attack[0].bit & may_x_ray){
        threat th = consider_x_ray(m.to_b, attack[0].bit);
        if (th.ind != -1){
          if (bitboards[black] & th.bit){
            defense.push_back(th);
            x_rays[3] = 1;
          }
          if (bitboards[white] & th.bit){
            attack.push_back(th);
            x_rays[0] = 1;
          }
        }
      }
      defense.erase(defense.begin());
    }
    if (x_rays[1]){
      x_rays[1] = 0;
      std::sort(attack.begin(), attack.end(), sort_threats_1);
    }
    if (x_rays[3]){
      x_rays[3] = 0;
      std::sort(defense.begin(), defense.end(), sort_threats_2);
    }
    if (x_rays[0]){
      x_rays[0] = false;
      x_rays[1] = true;
    }
    if (x_rays[2]){
      x_rays[2] = false;
      x_rays[3] = true;
    }
  }
  for (int c = d - 1; c > 0; c--){
    if (c % 2 == 1){
      gain[c - 1] = std::min(gain[c], gain[c - 1]);
    }
    else{ gain[c - 1] = max_(gain[c], gain[c - 1]);}
  }
  return gain[0];
}
