#ifndef BITFUNCTION_HPP
#define BITFUNCTION_HPP

#include <cmath>


typedef unsigned long long int bitboard;




extern const bitboard a_file, b_file, c_file, d_file, e_file, f_file, g_file, h_file, rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8;
extern const bitboard center, ext_center;
extern const bitboard one;
extern const bitboard white_camp, black_camp;






inline bitboard flip(bitboard const& bit){
  return ( (bit << 56)) |
        ( (bit << 40) & 0x00ff000000000000 ) |
        ( (bit << 24) & 0x0000ff0000000000 ) |
        ( (bit <<  8) & 0x000000ff00000000 ) |
        ( (bit >>  8) & 0x00000000ff000000 ) |
        ( (bit >> 24) & 0x0000000000ff0000 ) |
        ( (bit >> 40) & 0x000000000000ff00 ) |
        ( (bit >> 56) );
};

bitboard south_line(bitboard const& bit);
bitboard north_line(bitboard const& bit);
bitboard east_line(bitboard const& bit);
bitboard west_line(bitboard const& bit);

bitboard south_east_line(bitboard const& bit);
bitboard north_east_line(bitboard const& bit);
bitboard south_west_line(bitboard const& bit);
bitboard north_west_line(bitboard const& bit);


//following functions change a bitboard in a certain direction
//they aren't actually used but whatever

inline void south(bitboard& bit){ bit = (bit & ~rank_1) >> 8;}
inline void north(bitboard& bit){ bit = (bit & ~rank_8) << 8;}
inline void west(bitboard& bit){ bit = (bit & ~a_file) << 1;}
inline void east(bitboard& bit){ bit = (bit & ~h_file) >> 1;}

inline void south_east(bitboard& bit){ bit = (bit & ~(rank_1 | h_file)) >> 9;}
inline void south_west(bitboard& bit){ bit = (bit & ~(rank_1 | a_file)) >> 7;}
inline void north_east(bitboard& bit){ bit = (bit & ~(rank_8 | h_file)) << 7;}
inline void north_west(bitboard& bit){ bit = (bit & ~(rank_8 | a_file)) << 9;}

inline void south_south_east(bitboard& bit){ bit = (bit & ~(rank_1 | rank_2 | h_file)) >> 17;}
inline void south_east_east(bitboard& bit){ bit = (bit & ~(rank_1 | g_file | h_file)) >> 10;}
inline void south_south_west(bitboard& bit){ bit = (bit & ~(rank_1 | rank_2 | a_file)) >> 15;}
inline void south_west_west(bitboard& bit){ bit = (bit & ~(rank_1 | b_file | a_file)) >> 6;}
inline void north_north_east(bitboard& bit){ bit = (bit & ~(rank_7 | rank_8 | h_file)) << 15;}
inline void north_east_east(bitboard& bit){ bit = (bit & ~(rank_7 | g_file | h_file)) << 6;}
inline void north_north_west(bitboard& bit){ bit = (bit & ~(rank_7 | rank_8 | a_file)) << 17;}
inline void north_west_west(bitboard& bit){ bit = (bit & ~(rank_7 | b_file | b_file)) << 10;}


//following functions works like the ones above but returns another bitboard and leave the input one unchanged
//they are the base of this engine and used everywhere

inline bitboard S(bitboard const& bit){ return (bit & ~rank_1) >> 8;}
inline bitboard N(bitboard const& bit){ return (bit & ~rank_8) << 8;}
inline bitboard W(bitboard const& bit){ return (bit & ~a_file) << 1;}
inline bitboard E(bitboard const& bit){ return (bit & ~h_file) >> 1;}

inline bitboard S_E(bitboard const& bit){ return (bit & ~(rank_1 | h_file)) >> 9;}
inline bitboard S_W(bitboard const& bit){ return (bit & ~(rank_1 | a_file)) >> 7;}
inline bitboard N_E(bitboard const& bit){ return (bit & ~(rank_8 | h_file)) << 7;}
inline bitboard N_W(bitboard const& bit){ return (bit & ~(rank_8 | a_file)) << 9;}

inline bitboard S_S_E(bitboard const& bit){ return (bit & ~(rank_1 | rank_2 | h_file)) >> 17;}
inline bitboard S_E_E(bitboard const& bit){ return (bit & ~(rank_1 | g_file | h_file)) >> 10;}
inline bitboard S_S_W(bitboard const& bit){ return (bit & ~(rank_1 | rank_2 | a_file)) >> 15;}
inline bitboard S_W_W(bitboard const& bit){ return (bit & ~(rank_1 | b_file | a_file)) >> 6;}
inline bitboard N_N_E(bitboard const& bit){ return (bit & ~(rank_7 | rank_8 | h_file)) << 15;}
inline bitboard N_E_E(bitboard const& bit){ return (bit & ~(rank_8 | g_file | h_file)) << 6;}
inline bitboard N_N_W(bitboard const& bit){ return (bit & ~(rank_7 | rank_8 | a_file)) << 17;}
inline bitboard N_W_W(bitboard const& bit){ return (bit & ~(rank_8 | b_file | a_file)) << 10;}





void display_bitboard(bitboard bit);

int popcount(bitboard x);

inline int get_ind(bitboard const& x){ return std::log2(x);} //only use on single populated bitboards;


























#endif
