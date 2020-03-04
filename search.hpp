#ifndef SEARCH_HPP
#define SEARCH_HPP


#include "database.hpp"
#include "structs.hpp"
#include <vector>


bool sort_moves(move const& m1, move const& m2);



int alphabeta(int alpha, int beta, chessboard *C, int const& depth, database * D, int const& start_turn, eval_coefs coefs, int const& time_stop);

move timed_search(chessboard *C, int const& search_time, database* D, eval_coefs const& coefs, bool const& test);

void play_test(int depth);


#endif
