#include "structs.hpp"
#include <array>




//piece's square tables values initialization

//there is 4 for each piece, one for each gamephase even if some are very similar

std::array<std::array<std::array<int, 64>, 6>, 4> square_tables;

void init_ST(){
  square_tables[opening][pawn / 2 - 1] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    80, 85, 95, 100, 100, 95, 85, 80,
    80, 85, 90, 105, 105, 95, 85, 80,
    80, 85, 105, 115, 115, 105, 85, 80,
    90, 95, 115, 120, 120, 115, 95, 90,
    130, 160, 170, 170, 170, 170, 160, 130,
    200, 200, 200, 200, 200, 200, 200, 200,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  square_tables[opening][knight / 2 - 1] = {
    250, 265, 270, 270, 270, 270, 265, 250,
    275, 290, 295, 300, 300, 295, 290, 275,
    280, 300, 300, 305, 305, 300, 300, 280,
    290, 305, 305, 305, 305, 305, 305, 290,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 305, 305, 310, 310, 305, 305, 290,
    280, 295, 300, 300, 300, 300, 295, 280,
    250, 260, 270, 270, 270, 270, 260, 260
  };


  square_tables[opening][bishop / 2 - 1] = {
    285, 280, 290, 290, 290, 290, 280, 285,
    295, 315, 305, 305, 305, 305, 315, 295,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 315, 320, 325, 325, 320, 315, 290,
    290, 310, 320, 320, 320, 320, 310, 290,
    280, 300, 310, 315, 315, 310, 300, 280,
    280, 300, 300, 300, 300, 300, 300, 280,
    250, 270, 290, 290, 290, 290, 270, 250
  };

  square_tables[opening][rook / 2 - 1] = {
    485, 485, 495, 505, 505, 495, 485, 485,
    460, 480, 505, 505, 505, 505, 480, 460,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 510, 510, 500, 500, 480,
    510, 530, 530, 530, 530, 530, 530, 510,
    450, 480, 480, 480, 480, 480, 480, 450
  };

  square_tables[opening][queen / 2 - 1] = {
    850, 875, 900, 900, 900, 875, 875, 850,
    860, 875, 895, 900, 900, 895, 875, 860,
    870, 885, 900, 900, 900, 900, 885, 870,
    850, 875, 890, 890, 890, 890, 875, 850,
    840, 860, 875, 875, 875, 875, 860, 840,
    820, 830, 850, 870, 870, 850, 830, 820,
    800, 800, 830, 860, 860, 830, 800, 800,
    800, 800, 830, 860, 860, 830, 800, 800
  };

  square_tables[opening][king / 2 - 1] = {
    9990, 10000, 9980, 9990, 9950, 9960, 10000, 10000,
    9970, 9970, 9950, 9900, 9900, 9900, 9980, 9950,
    9800, 9700, 9600, 9500, 9500, 9600, 9700, 9800,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500
  };






  square_tables[midgame][pawn / 2 - 1] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    90, 95, 100, 100, 100, 100, 95, 90,
    90, 95, 95, 105, 105, 95, 95, 90,
    95, 95, 105, 110, 110, 105, 95, 95,
    95, 105, 115, 120, 120, 115, 105, 95,
    130, 160, 170, 170, 170, 170, 160, 130,
    200, 200, 200, 200, 200, 200, 200, 200,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  square_tables[midgame][knight / 2 - 1] = {
    250, 260, 270, 270, 270, 270, 260, 250,
    275, 290, 295, 300, 300, 295, 290, 275,
    280, 300, 305, 305, 305, 305, 300, 280,
    290, 305, 310, 315, 315, 310, 305, 290,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 310, 315, 315, 315, 315, 310, 290,
    290, 315, 320, 320, 320, 320, 315, 290,
    250, 260, 270, 270, 270, 270, 260, 260
  };


  square_tables[midgame][bishop / 2 - 1] = {
    285, 280, 290, 290, 290, 290, 280, 285,
    295, 315, 305, 305, 305, 305, 315, 295,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 315, 320, 325, 325, 320, 315, 290,
    290, 310, 320, 320, 320, 320, 310, 290,
    280, 300, 320, 325, 325, 320, 300, 280,
    280, 300, 310, 315, 315, 310, 300, 280,
    250, 270, 290, 290, 290, 290, 270, 250
  };

  square_tables[midgame][rook / 2 - 1] = {
    485, 485, 495, 505, 505, 495, 485, 485,
    460, 480, 505, 505, 505, 505, 480, 460,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 500, 500, 500, 500, 480,
    480, 500, 500, 510, 510, 500, 500, 480,
    510, 530, 530, 530, 530, 530, 530, 510,
    450, 480, 480, 480, 480, 480, 480, 450
  };

  square_tables[midgame][queen / 2 - 1] = {
    830, 850, 860, 870, 870, 860, 850, 830,
    860, 880, 895, 895, 895, 895, 880, 860,
    870, 895, 900, 905, 905, 900, 895, 870,
    880, 900, 905, 905, 905, 905, 900, 880,
    870, 895, 905, 905, 905, 905, 895, 870,
    860, 890, 900, 900, 900, 900, 890, 860,
    850, 880, 895, 895, 895, 895, 880, 850,
    830, 850, 860, 860, 860, 860, 850, 830
  };

  square_tables[midgame][king / 2 - 1] = {
    9990, 10000, 9980, 9950, 9950, 9960, 10000, 10000,
    9970, 9970, 9950, 9900, 9900, 9900, 9980, 9950,
    9800, 9700, 9600, 9500, 9500, 9600, 9700, 9800,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500,
    9500, 9500, 9500, 9500, 9500, 9500, 9500, 9500
  };









  square_tables[early_end][pawn / 2 - 1] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    90, 90, 90, 90, 90, 90, 90, 90,
    90, 95, 95, 100, 100, 95, 95, 90,
    95, 95, 105, 110, 110, 105, 95, 95,
    95, 105, 115, 120, 120, 115, 105, 95,
    130, 160, 170, 170, 170, 170, 160, 130,
    250, 250, 250, 250, 250, 250, 250, 250,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  square_tables[early_end][knight / 2 - 1] = {
    250, 260, 270, 270, 270, 270, 260, 250,
    275, 290, 295, 300, 300, 295, 290, 275,
    280, 300, 305, 305, 305, 305, 300, 280,
    290, 305, 310, 315, 315, 310, 305, 290,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 310, 315, 315, 315, 315, 310, 290,
    290, 315, 320, 320, 320, 320, 315, 290,
    250, 260, 270, 270, 270, 270, 260, 260
  };


  square_tables[early_end][bishop / 2 - 1] = {
    285, 280, 290, 290, 290, 290, 280, 285,
    295, 315, 305, 305, 305, 305, 315, 295,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 315, 320, 325, 325, 320, 315, 290,
    290, 310, 320, 320, 320, 320, 310, 290,
    280, 300, 320, 325, 325, 320, 300, 280,
    280, 300, 310, 315, 315, 310, 300, 280,
    250, 270, 290, 290, 290, 290, 270, 250
  };

  square_tables[early_end][rook / 2 - 1] = {
    490, 495, 495, 495, 495, 495, 495, 490,
    500, 505, 505, 505, 505, 505, 505, 500,
    490, 500, 500, 500, 500, 500, 500, 490,
    490, 500, 500, 500, 500, 500, 500, 490,
    490, 500, 500, 500, 500, 500, 500, 490,
    500, 505, 505, 505, 505, 505, 505, 500,
    505, 510, 515, 515, 515, 515, 510, 505,
    495, 495, 500, 500, 500, 500, 495, 495,

  };

  square_tables[early_end][queen / 2 - 1] = {
    880, 890, 900, 900, 900, 900, 890, 880,
    895, 900, 905, 905, 905, 905, 900, 895,
    900, 910, 915, 915, 915, 915, 910, 900,
    900, 910, 915, 920, 920, 915, 910, 900,
    905, 915, 925, 925, 925, 925, 915, 905,
    900, 910, 915, 920, 920, 915, 910, 900,
    900, 905, 915, 915, 915, 915, 905, 900,
    880, 895, 900, 900, 900, 900, 895, 880
  };

  square_tables[early_end][king / 2 - 1] = {
    9980, 9985, 999, 9990, 9990, 9990, 9985, 9980,
    9980, 9985, 9990, 9990, 9990, 9990, 9985, 9980,
    9990, 9995, 10000, 10000, 10000, 10000, 9995, 9990,
    9980, 9990, 9990, 9990, 9990, 9990, 9990, 9980,
    9970, 9980, 9990, 9990, 9990, 9990, 9980, 9970,
    9960, 9975, 9980, 9980, 9980, 9980, 9975, 9960,
    9950, 9960, 9970, 9970, 9970, 9970, 9960, 9950,
    9950, 9960, 9970, 9970, 9970, 9970, 9960, 9950,
  };








  square_tables[endgame][pawn / 2 - 1] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    90, 90, 90, 90, 90, 90, 90, 90,
    90, 95, 95, 100, 100, 95, 95, 90,
    95, 95, 105, 110, 110, 105, 95, 95,
    95, 105, 115, 120, 120, 115, 105, 95,
    130, 160, 170, 180, 180, 170, 160, 130,
    250, 250, 250, 250, 250, 250, 250, 250,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  square_tables[endgame][knight / 2 - 1] = {
    250, 260, 270, 270, 270, 270, 260, 250,
    275, 290, 295, 300, 300, 295, 290, 275,
    280, 300, 305, 305, 305, 305, 300, 280,
    290, 305, 310, 315, 315, 310, 305, 290,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 310, 315, 315, 315, 315, 310, 290,
    290, 315, 320, 320, 320, 320, 315, 290,
    250, 260, 270, 270, 270, 270, 260, 260
  };


  square_tables[endgame][bishop / 2 - 1] = {
    285, 280, 290, 290, 290, 290, 280, 285,
    295, 315, 305, 305, 305, 305, 315, 295,
    295, 310, 315, 315, 315, 315, 310, 295,
    290, 315, 320, 325, 325, 320, 315, 290,
    290, 310, 320, 320, 320, 320, 310, 290,
    280, 300, 320, 325, 325, 320, 300, 280,
    280, 300, 310, 315, 315, 310, 300, 280,
    250, 270, 290, 290, 290, 290, 270, 250
  };

  square_tables[endgame][rook / 2 - 1] = {
    495, 500, 500, 500, 500, 500, 500, 495,
    495, 500, 500, 500, 500, 500, 500, 495,
    495, 500, 500, 500, 500, 500, 500, 495,
    495, 500, 500, 500, 500, 500, 500, 495,
    495, 500, 500, 500, 500, 500, 500, 495,
    495, 500, 500, 500, 500, 500, 500, 495,
    500, 505, 505, 505, 505, 505, 505, 500,
    495, 500, 500, 500, 500, 500, 500, 495
  };

  square_tables[endgame][queen / 2 - 1] = {
    900, 905, 910, 910, 910, 910, 905, 900,
    905, 910, 915, 920, 920, 915, 910, 905,
    910, 920, 930, 930, 930, 930, 920, 910,
    910, 920, 930, 930, 930, 930, 920, 910,
    910, 920, 930, 930, 930, 930, 920, 910,
    910, 920, 930, 930, 930, 930, 920, 910,
    905, 910, 915, 920, 920, 915, 910, 905,
    900, 905, 910, 910, 910, 910, 905, 900
  };

  square_tables[endgame][king / 2 - 1] = {
    9930, 9940, 9965, 9970, 9970, 9965, 9940, 9930,
    9940, 9960, 9975, 9980, 9980, 9975, 9960, 9940,
    9960, 9980, 9990, 9995, 9995, 9990, 9980, 9960,
    9970, 9990, 9995, 10000, 10000, 9995, 9990, 9970,
    9970, 9990, 9995, 10000, 10000, 9995, 9990, 9970,
    9960, 9980, 9990, 9995, 9995, 9990, 9980, 9960,
    9940, 9960, 9975, 9980, 9980, 9975, 9960, 9940,
    9930, 9940, 9965, 9970, 9970, 9965, 9940, 9930
  };

}


//used to get ST value for black pieces
const std::array<int, 64> flip_mailbox = {
  56, 57, 58, 59, 60, 61, 62, 63,
  48, 49, 50, 51, 52, 53, 54, 55,
  40, 41, 42, 43, 44, 45, 46, 47,
  32, 33, 34, 35, 36, 37, 38, 39,
  24, 25, 26, 27, 28, 29, 30, 31,
  16, 17, 18, 19, 20, 21, 22, 23,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7
};
