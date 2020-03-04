Alphabetise is a chess engine i developped in the last few years.

I would like to thanks the chessprogramming wiki community, who taught me a lot about chess engines.
https://www.chessprogramming.org/Main_Page
a lot of terms specifics to chessprogramming that i'm gonna use are explained on this site (quiescence, late move reduction...).

It is a single thread engine based on the alphabeta tree search, with transposition tables, quiescence search, simple move ordering and late move reduction.
The chessboard is represented by bitboards (64 bit words, one for each piece type) witch allows the massive use of binary operators to do chess related stuffs and gain a lot of time.

In practice, it can go to depth of 7 / 8 in a reasonnable amount of time (depending on the position complexity), without taking quiescence search into consideration.
the main search function uses iterative deepening with aspiration window, and stops the search after a predetermined amount of time.

the evaluation function takes about 50 int parameters witch are the evaluation weights, the goal was to use genetic algorithms in order to optimise the evaluation.
Infortunatelly the results were pretty inconsistents and i figured the evaluation probably still have bugs so i stopped it for now and just went for eval weights that seems good to me.

It does not use any oracle such as an opening book or endgame database.

Alphabetise is far from perfect, it struggle a bit in the endgame, might move the same piece to much in the opening and sometimes blunder an obvious tactics like a pawn fork or whatever, but it clearly knows how to play!
also once i spotted a bug when it played a bad move thinking it was 100% winning (evaluation at +150000, witch is not supposed to be reachable). didn't looked for the problem.
My ELO rating is around 1400 and Alphabetise kicks my ass almost every time (either i get destroyed in the midgame or i manage to survive untill the endgame and then i have my chances).
It probably wouldn't beat any grandmaster, but it would destroy any begginer.
