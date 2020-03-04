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








int conv(std::string s){
  int a = -1; int b = -1;
  if (s[0] == 'h'){ a = 1;}
  if (s[0] == 'g'){ a = 2;}
  if (s[0] == 'f'){ a = 3;}
  if (s[0] == 'e'){ a = 4;}
  if (s[0] == 'd'){ a = 5;}
  if (s[0] == 'c'){ a = 6;}
  if (s[0] == 'b'){ a = 7;}
  if (s[0] == 'a'){ a = 8;}
  if (s[1] == '1'){ b = 1;}
  if (s[1] == '2'){ b = 2;}
  if (s[1] == '3'){ b = 3;}
  if (s[1] == '4'){ b = 4;}
  if (s[1] == '5'){ b = 5;}
  if (s[1] == '6'){ b = 6;}
  if (s[1] == '7'){ b = 7;}
  if (s[1] == '8'){ b = 8;}
  return ((b - 1) * 8 + a - 1);
}


move get_human_move(chessboard C){
  std::string c1, c2;
  bool stop = false;
  std::array<int, 2> m;
  move M;
  C.get_dirgolem();
  std::vector<move> moves = C.get_all_moves();
  while (!stop){
    std::cout << "give your move" << std::endl;
    std::cin >> c1;
    if (c1[0] == 'o'){
      if (c1 == "o0"){
        m = {3, 1};
      }
      if (c1 == "o1"){
        m = {3, 5};
      }
    }
    else{
      std::cin >> c2;
      int i = conv(c1);
      int j = conv(c2);
      m = {i, j};
    }

    for (std::vector<move>::iterator it = moves.begin(); it != moves.end(); ++it){
      if ((*it).from == m[0] && (*it).to == m[1]){
        M = *it;
        stop = true;
        break;
      }
    }

    if (!stop){
      std::cout << "move illégal, les moves légaux sont :" << std::endl;
      for (auto it = moves.begin(); it != moves.end(); ++it){
        display_move(*it);
      }
    }
  }
  return M;
}


//few test functions

void test_move_gen(){
  for (int k = 0; k < 1000; k++){
    chessboard C;
    for (int c= 0; c < 40; c++){
      std::vector<move> m1, m2, m3;
      C.get_dirgolem();
      m1 = C.get_all_moves();
      m2 = C.stage_1_moves();
      m3 = C.stage_2_moves();
      m2.insert(m2.end(), m3.begin(), m3.end());
      m3 = C.stage_3_moves();
      m2.insert(m2.end(), m3.begin(), m3.end());
      m3 = C.stage_4_moves();
      m2.insert(m2.end(), m3.begin(), m3.end());
      if (!C.in_check){
        if (m1.size() != m2.size()){
          std::cout << "ERROR START //////////////////////////" << std::endl << std::endl;
          C.display_all();
          std::cout << "full generation : " << m1.size() << std::endl;
          for (auto it = m1.begin(); it != m1.end(); ++it){
            display_move(*it);
          }

          std::cout << std::endl << "staged generation :" << m2.size() << std::endl;
          for (auto it = m2.begin(); it != m2.end(); ++it){
            display_move(*it);
          }
          if (m1.size() < m2.size()){
            std::cout << "missing :" << std::endl;
            for (auto it = m2.begin(); it != m2.end(); ++it){
              bool found = false;
              for (auto it2 = m1.begin(); it2 != m1.end(); ++it2){
                if ((*it).from == (*it2).from && (*it).to == (*it2).to && (*it).type == (*it2).type){
                  found = true;
                  break;
                }
              }
              if (!found){ display_move(*it);}
            }
          }
          std::cout << std::endl << "XXXXXXXXXXX  END  XXXXXXXXXXXXX" << std::endl << std::endl;
        }
      }
      if (m1.size() == 0){ break;}
      int a = rand() % m1.size();
      C.make_move(m1[a]);
    }
  }
}

void test_see(){
  chessboard C;
  while(true){
    C.display(true);
    C.get_dirgolem();
    std::vector<move> moves = C.get_all_moves();
    for (auto it = moves.begin(); it != moves.end(); ++it){
      display_move(*it);
      std::cout << C.SEE(*it) << std::endl;
    }

    C.make_move(get_human_move(C));
  }
}





void hash_finder(chessboard C){//very usefull for debuging, helps find where an error occured in the search tree
  while(true){
    C.make_move(get_human_move(C));
    C.display(true);
    std::cout << *(C.reached_pos.end() - 1) << std::endl;
  }
}




int main(){

  srand(time(NULL));

  init_ST();

  std::cout << "hello, welcome to play against alphabetise, my chess engine." << std::endl << std::endl;
  std::cout << "it is a very simple engine, not a strong player, but it does know how to play." << std::endl;
  std::cout << "for more informations about the engine, refere to the READ_ME.txt file." << std::endl << std::endl;
  std::cout << "moves are given by writing the origin square, then press enter, then the target square and enter again" << std::endl;
  std::cout << "if you want to play the move 1.e4 for example, you must write e2, then press enter, then write e4 and enter again." << std::endl;
  std::cout << "only exeption is for castle, just write o0 for king side and o1 for queen side." << std::endl << std::endl;
  std::cout << "how mutch time per move do you want to give the engine (in seconds, minimum 2) ?" << std::endl;
  int time_per_move;
  std::cin >> time_per_move;
  play_test(time_per_move);



}
