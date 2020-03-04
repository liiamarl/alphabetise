#include "database.hpp"
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




database::database(){//just creates empty database
  for (auto it = history.begin(); it != history.end(); ++it){
    for (auto it2 = (*it).begin(); it2 != (*it).end(); ++it2){
      (*it2) = 0;
    }
  }
  for (auto it = data.begin(); it != data.end(); ++it){
    (*it).best_move = 0;
    (*it).full_hash = 0;
  }
}


pos_info database::get_info(bitboard hash){//get the infos about a position with the input hash value
  n_acces++;
  n_find++;
  int ind = hash & 0b111111111111111;
  int ind_2 = (hash + 1) & 0b111111111111111;//for bucketization
  if (hash == data[ind].full_hash){ return data[ind];}
  if (hash == data[ind_2].full_hash){ return data[ind_2];}
  pos_info p;
  p.best_move = 0;//mean that the pos wasn't found
  n_find--;
  return p;
}


bool operator> (pos_info const& p1, pos_info const& p2){//replacement strategy
  if (p1.base_turn == p2.base_turn){ return p1.search_depth > p2.search_depth;}
  return p1.base_turn > p2.base_turn;
}


void database::give_info(pos_info p){//after a position has been searched, infos are given to the database
  int ind = p.full_hash & 0b111111111111111;
  int ind_2 = (p.full_hash + 1) & 0b111111111111111;
  if (!data[ind].best_move){ data[ind] = p;}
  else if(!data[ind_2].best_move){ data[ind_2] = p;}
  else if (p > data[ind]){ data[ind] = p;}
  else if(p > data[ind_2]){ data[ind_2] = p;}
}


void database::display_stats(){//juste to check if it's working, it helped find important bugs
  int tot = 0;
  int occup = 0;
  for (auto it = data.begin(); it != data.end(); ++it){
    tot++;
    if ((*it).best_move){ occup++;}
  }

  float hit_rate = 100.0 * n_find / n_acces;
  float occupancy = 100.0 * occup / tot;
  std::cout << n_find << " TT hits (" << hit_rate << "%)" << std::endl;
  std::cout << "TT occupancy : " << occupancy << "%" << std::endl;
}


void database::reset_stats(){
  n_acces = 0;
  n_find = 0;
}
