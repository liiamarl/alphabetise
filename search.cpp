#include "bit_function.hpp"
#include <iostream>
#include "structs.hpp"
#include "chessboard.hpp"
#include <array>
#include <cstdlib>
#include <ctime>
#include "search.hpp"
#include <algorithm>
#include "database.hpp"
#include <cmath>





bool sort_moves(move const& m1, move const& m2){ return m1.eval > m2.eval;};












//recursive alphabeta with transposition tables, late move reduction and quiescence search



int alphabeta(int alpha, int beta, chessboard *C, int const& depth, database * D, int const& start_turn, eval_coefs coefs, int const& time_stop){

  C-> get_dirgolem();

  if (depth > 0){
    pos_info p = D-> get_info(*(C-> reached_pos.end() - 1));
    if (p.best_move){
      if (p.eval > beta){ return beta;}
    }
    std::vector<move> moves = C-> moves_in_order(p.best_move);
    p.search_depth = depth;
    p.base_turn = start_turn;
    p.full_hash = *(C-> reached_pos.end() - 1);
    for (auto it = moves.begin(); it != moves.end(); ++it){
      changes ch = C-> make_move(*it);
      (*it).eval = -alphabeta(-beta, -alpha, C, depth - 1 - (*it).depth_reduction, D, start_turn, coefs, time_stop);
      if ((*it).depth_reduction > 0 && (*it).eval > alpha){
        (*it).eval = -alphabeta(-beta, -alpha, C, depth - 1, D, start_turn, coefs, time_stop);
      }
      C-> unmake_move(*it, ch);
      if ((*it).eval > alpha){
        alpha = (*it).eval;
        p.eval = alpha;
        p.best_move = (*it).to_b | (*it).from_b;
        if (alpha > beta){
          D-> give_info(p);
          return beta;
        }
      }
      if (time(NULL) > time_stop - 1){ return alpha;}
    }
    D-> give_info(p);
    return alpha;
  }


  if (depth > -4){
    if (!C-> in_check){
      int pos_eval = C-> get_eval(coefs);
      if (pos_eval > alpha){
        alpha = pos_eval;
        if (alpha > beta){ return beta;}
      }
    }
    std::vector<move> moves = C-> quiescence_moves();
    for (auto it = moves.begin(); it != moves.end(); ++it){
      changes ch = C-> make_move(*it);
      (*it).eval = -alphabeta(-beta, -alpha, C, depth - 1, D, start_turn, coefs, time_stop);
      C-> unmake_move(*it, ch);
      if ((*it).eval > alpha){
        alpha = (*it).eval;
        if (alpha > beta){ return beta;}
      }
      if (time(NULL) > time_stop - 1){ return alpha;}
    }
    return alpha;
  }
  int pos_eval = C-> get_eval(coefs);
  if (depth < -8){ return pos_eval;}
  if (pos_eval > alpha){
    alpha = pos_eval;
    if (alpha > beta){ return beta;}
  }
  std::vector<move> moves = C-> final_moves();
  for (auto it = moves.begin(); it != moves.end(); ++it){
    changes ch = C-> make_move(*it);
    (*it).eval = -alphabeta(-beta, -alpha, C, depth - 1, D, start_turn, coefs, time_stop);
    C-> unmake_move(*it, ch);
    if ((*it).eval > alpha){
      alpha = (*it).eval;
      if (alpha > beta){ return beta;}
    }
    if (time(NULL) > time_stop - 1){ return alpha;}
  }

  return alpha;
}






//iterative deepening with aspiration window
//the window starts at +-80 centipawns, and just go to +- infinity if it fails
move timed_search(chessboard *C, int const& search_time, database* D, eval_coefs const& coefs){
  int time_stop = time(NULL) + search_time;
  C-> get_dirgolem();
  std::vector<move> moves = C-> get_all_moves();
  for (auto it = moves.begin(); it != moves.end(); ++it){
    changes ch = C-> make_move(*it);
    (*it).eval = -alphabeta(-150000, 150000, C, 2, D, C-> turns, coefs, time_stop) - 20;
    C-> unmake_move(*it, ch);
    (*it).search_depth = 2;
  }
  std::sort(moves.begin(), moves.end(), sort_moves);

  std::cout << "pre-evaluation done" << std::endl;

  int search_size = moves.size();
  if (search_size > 4){ search_size = 4;}
  int depth = 2;
  move best_move;
  std::vector<move> best_line;
  while(true){
    depth++;
    move best;
    best.eval = -150000;
    for (int c = 0; c < search_size; c++){
      if (time(NULL) > time_stop - 1){ break;}
      changes ch = C-> make_move(moves[c]);
      int alpha = moves[c].eval - 80;
      int beta = moves[c].eval + 80;
      moves[c].eval = -alphabeta(-beta, -alpha, C, depth, D, C-> turns, coefs, time_stop);
      if (moves[c].eval == alpha){//fails low
        moves[c].eval = -alphabeta(-beta, 150000, C, depth, D, C-> turns, coefs, time_stop);
      }
      if (moves[c].eval == beta){//fails high
        moves[c].eval = -alphabeta(-150000, -alpha, C, depth, D, C-> turns, coefs, time_stop);
      }
      if (moves[c].eval > best.eval){ best = moves[c];}
      moves[c].search_depth = depth;
      C-> unmake_move(moves[c], ch);
    }
    std::sort(moves.begin(), moves.end(), sort_moves);


    std::cout << "depth " << depth + 1 << " : " << moves[0].eval << "  ";
    display_move(moves[0]);
    best_move = best;
    if (time(NULL) > time_stop - 1){ break;}
  }


  return best_move;
}





//main function to play against the engine
void play_test(int search_time){
  eval_coefs coefs;
  coefs.open_coefs = {10, 4, 15, 15, 15, 10, 5, 10, 15, 10, 15, 10, 5, 10, 25, 10, 1, 1, 8, 10, 5, 5, 5, 2, 1};
  coefs.midgame_coefs = {10, 4, 20, 25, 20, 30, 7, 20, 20, 10, 15, 10, 5, 20, 15, 10, 1, 2, 15, 10, 5, 4, 4, 3, 3};
  coefs.early_end_coefs = {12, 3, 2, 30, 20, 10, 15, 3, 15, 5, 7, 15, 3, 30, 15, 15, 2, 2, 5, 15, 3, 3, 2, 2, 2, 20, 20};
  coefs.endgame_coefs = {15, 2, 2, 40, 15, 12, 10, 5, 8, 15, 3, 40, 15, 15, 2, 2, 5, 15, 2, 2, 2, 2, 3, 20, 30};
  chessboard C;
  database D;
  while(true){
    C.display(true);
    std::cout << C.get_eval(coefs) << std::endl;
    C.make_move(get_human_move(C));
    C.make_move(timed_search(&C, search_time, &D, coefs));
    D.display_stats();
    D.reset_stats();
  }
}
