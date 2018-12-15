/*
  Xiphos, a UCI chess engine
  Copyright (C) 2018 Milos Tatarevic

  Xiphos is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Xiphos is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "eval.h"
#include "game.h"

int pst_mid[P_LIMIT][BOARD_SIZE], pst_end[P_LIMIT][BOARD_SIZE];
int distance[BOARD_SIZE][BOARD_SIZE];

const int piece_value[N_PIECES] = { 100, 310, 330, 500, 1000, 20000 };
const int piece_phase[N_PIECES] = { 0, 6, 6, 13, 28, 0 };

const int pst_mid_base[N_PIECES][BOARD_SIZE] = {
  {
    0,   0,   0,   0,   0,   0,   0,   0,
   -7,   3, -21,   4,  32,  45,  -8, -47,
  -17, -15,  12,   8,  56,  35,   5,  -9,
  -17,  -3, -16,  -6,  -8, -11,  -4, -18,
  -24, -32, -12, -20, -13, -14, -27, -41,
  -34, -25, -32, -36, -23, -28, -11, -41,
  -33, -14, -25, -21, -22,  -4,  15, -28,
    0,   0,   0,   0,   0,   0,   0,   0
  }, {
 -143, -82, -56, -40,   2,-111,-105, -87,
  -10,   2,  70,  53,  38,  75,  34,  22,
    7,  42,  46,  55,  86, 103,  61,  31,
   43,  59,  59,  74,  65,  77,  60,  71,
   49,  43,  62,  66,  74,  75,  77,  60,
   30,  38,  38,  46,  55,  58,  58,  39,
   17,  18,  33,  40,  49,  41,  50,  39,
  -18,  37,  14,  38,  50,  34,  35,   7
  }, {
   25,  -8, -74, -80, -65, -87, -18,  -7,
   -7,   9,   9, -19,  26,  23,  14, -31,
   13,  17,  28,  43,  63,  57,  27,  34,
   15,  47,  33,  54,  45,  42,  54,  20,
   28,  25,  36,  56,  63,  44,  43,  53,
   30,  41,  35,  34,  37,  47,  52,  36,
   46,  39,  38,  26,  31,  47,  58,  47,
   29,  40,  29,  22,  31,  17,  22,  43
  }, {
    4,   3, -10,  24,  25, -26,   1,  12,
   -6, -18,  12,  31,   5,  38, -27,   7,
  -10,   6,  -4,  12,  27,  25,  22, -11,
  -14,  -6,  12,   7,  19,  28,  -5,   1,
  -22, -24,  -8,   0,   8,   8,  20, -14,
  -20,  -2,  -7,   2,   1,   5,  19, -11,
  -12,  -9,  -3,   5,  10,   9,  11, -47,
    0,   5,   7,  22,  20,  18,  -8,   3
  }, {
  -38, -11,  28,  20,  35,  60,  47,  32,
  -26, -34,  -2, -24, -49,   7, -13,  16,
  -10,   3,   4,  10,  18,  43,  32,  25,
  -16,   6,   5, -12,  14,  24,  23,  18,
   20,   5,   9,   4,  27,  18,  35,  23,
    4,  22,  10,  15,  15,  22,  39,  18,
    8,  21,  29,  23,  23,  38,  35,  21,
   13,  13,  17,  27,  16,   9,  -5,   8
  }, {
   -1,  42,  69, -16, -61, -59,  63,  86,
   22,  74,  39,  35,  21,  46, -21, -35,
  -11,  53, 133,  38,  64, 138,  59, -34,
  -47,  28,  53,   2,  14,  36,  16,-130,
  -60,  30,  12, -20, -21,  -8,   0,-101,
  -65, -10, -23, -26, -39, -28,  -2, -39,
    5,  -1,  -3, -52, -40, -19,  11,  18,
    7,  22,   7, -44,   7, -31,  24,  12
  }
};

const int pst_end_base[N_PIECES][BOARD_SIZE] = {
  {
    0,   0,   0,   0,   0,   0,   0,   0,
   13,  15,  24,  28,  36,  19,  27,  17,
    7,   5,   0,   2,  16,  -1,  19,  10,
    3,  -8, -11, -20, -15,  -9,  -1,   0,
   -6,  -5, -20, -24, -20, -15,  -9, -14,
  -13, -11, -15, -12, -14, -10, -16, -18,
  -13, -10,  -8, -11,  -1,  -3,  -8, -23,
    0,   0,   0,   0,   0,   0,   0,   0
  }, {
  -50, -16, -22, -32, -39, -38, -31, -68,
  -17,  -7, -35, -20, -11, -20, -12, -46,
  -11, -18,   9,  11,   8,  -3,  -4, -31,
  -11,  -9,  13,  20,  16,  15,   0, -19,
  -12,   0,  13,  16,  17,  14,   1,  -8,
  -21, -11,   5,   9,   9,  -3, -14, -13,
  -35,  -8, -16,   0,  -6, -20,  -5, -23,
  -31, -25, -15,  -5, -21, -15, -19, -45
  }, {
   -1, -14, -21, -13, -14,  -1, -20, -25,
   -6,  -7, -11, -14, -10, -16, -10, -14,
  -12,  -7,  -4,  -1,   2,   4,  -1,  -8,
   -4,  -3,   1,  15,  14,   5,  -3, -17,
   -3,  -4,   8,  13,  10,   8, -11, -20,
  -10,  -1,   3,   6,   3,   4,  -7,  -7,
  -12, -15, -11,  -4,  -5, -10, -15, -31,
  -12, -14,  -6,  -9,  -9,  -2, -28, -25
  }, {
   27,  33,  33,  34,  33,  34,  38,  41,
   30,  36,  37,  35,  27,  33,  41,  29,
   27,  30,  31,  30,  26,  32,  18,  19,
   28,  30,  25,  30,  24,  21,  24,  24,
   35,  33,  29,  30,  23,  21,  19,  19,
   28,  28,  34,  19,  23,  15,  16,  18,
   20,  22,  29,  23,  18,  19,  10,  19,
   26,  20,  25,  14,  17,  15,  23,  13
  }, {
   22,  37,  35,  41,  23,  56,  37,  36,
   33,  48,  47,  62,  77,  71,  53,  43,
   17,  28,  23,  56,  63,  46,  32,  17,
   45,  33,  52,  60,  61,  46,  62,  37,
   16,  45,  46,  56,  44,  48,  48,  39,
   23,  20,  42,  27,  41,  47,  28,  25,
   11,   5,  -1,  21,  21, -12, -18,  -6,
    1, -10,  -8, -18,   8, -11,  -5,  10
  }, {
  -50, -27, -19,  -8,  -6, -23, -45, -39,
   -6,   7,  -6,  -5,   4,  13,  15, -11,
    1,   3,  11,  12,  12,  17,  12,   5,
   -4,   9,   7,  13,   8,  18,  14,   3,
    0,   6,  12,  15,  14,   9,   7,  -4,
    2,   1,  10,   8,  11,  13,   4,  -7,
  -12,  -4,   5,   6,   8,   9,   3, -13,
  -35, -26,  -5,  -7, -20,  -3, -15, -40
  }
};

const int pawn_shield[BOARD_SIZE] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    4,  21,  19,   4,   9,  18,  12,   3,
   16,  26,   4,   3,   8,   5,   6,  10,
    4,  -2,   1,   5,   1,  -6,  -8,   6,
   13,  -4,   7,  -7, -13,   0,  -4,  -3,
   35,  95,  57, -24, -44, -19,  17,  -2,
   17,   7,  59,  34,  46,  12, -28, -35,
  -28, -31, -14,  -7,  -7, -11, -17, -26
};

const int pawn_storm[2][BOARD_SIZE] = {
  {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
  -29, -79, -13, -51, -39,  -4, -17,  -7,
   22,   2,   2, -11,  -3,   7,  14,  24,
   26,   1,   1,   3,   6,  -5,  -1,   9,
   16,  34,   0,   6,  15,  -6,   5,   5,
   12,  22,  -4,   2,   0,   4,  -3,   6,
    0,   0,   0,   0,   0,   0,   0,   0
  }, {
    0,   0,   0,   0,   0,   0,   0,   0,
   30,  22,   3,  -5, -70, -58,  13,  43,
  -28,-133, -42,  -6, -58, -64, -56, -21,
  -15,  20, -25, -20, -13, -18,  -7,  -7,
   15,   5, -12,  -8,  -7, -11,   6,  -8,
   14,  39,   5,   8,   8,   2,  20,  16,
   12,  23,  -2,   2,  -4,  10,   3,   6,
    0,   0,   0,   0,   0,   0,   0,   0
  }
};

const int connected_pawns[2][N_PHASES][BOARD_SIZE] = {
  {
    {
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      6,  15, -13,  19,  40,  23,  10,  27,
     -1,  -4,   4,  13,  27,  40,  21,  15,
     -6,  19,   8,   7,  13,  12,  25,  14,
      5,  12,  15,  14,  16,   8,  27,  13,
     10,  -2,   3,  -1,   2,  -4,  10,   1,
      0,   0,   0,   0,   0,   0,   0,   0
    }, {
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     29,  19,  58,  29,  35,  40,  29,  23,
      8,  14,  -1,  -5,   9,   2,   5,  -5,
      4,   2,   4,   9,   3,   3,  -2,   2,
     -1,   4,   9,   6,  13,   5,   1,  -2,
     -5,   1,   0,   4,   7,  -8,  -1, -20,
      0,   0,   0,   0,   0,   0,   0,   0
    }
  }, {
    {
      0,   0,   0,   0,   0,   0,   0,   0,
    148,  99, 136, 134, 169, 123,  54,  35,
     28,  36,  72,  44,  46,  42,  76,  34,
     46,  37,  41,  36,  54,  32,  26,  39,
     24,  32,  23,  31,  33,  21,  36,  22,
     18,  29,  21,  31,  31,  19,  37,  29,
     18,   5,   9,  21,  16,  -2,  11,   9,
      0,   0,   0,   0,   0,   0,   0,   0
    }, {
      0,   0,   0,   0,   0,   0,   0,   0,
     41,  77,  35,  72,  93,  80,  58,  56,
     77,  64,  65,  56,  52,  52,  25,  54,
     26,  33,  36,  34,  32,  25,  23,  15,
      3,   6,  16,  23,  19,  10,   5,   8,
     -3,   6,   8,   5,   8,  -2,   1,  -8,
    -16,  -7,  -9,  -1,   1,  -8,  -2, -24,
      0,   0,   0,   0,   0,   0,   0,   0
    }
  }
};

const int passer_bonus[N_PHASES][BOARD_SIZE] = {
  {
    0,   0,   0,   0,   0,   0,   0,   0,
   94,  85,  87,  82, 102,  78,  82, 106,
   51,  41,  20,  34,   7,  25,   3,  25,
   21,  12,  15,  12,  10,   6,  28,   3,
   16,   2, -22,  -3,  -9,   0, -17,  11,
   -4,  -8, -22,  -7, -17,  -5, -28,  16,
   -6,  -9, -10,   5,  15, -19,  -5,   6,
    0,   0,   0,   0,   0,   0,   0,   0
  }, {
    0,   0,   0,   0,   0,   0,   0,   0,
  109, 112, 113, 111, 107, 114, 112, 117,
   63,  67,  75,  71,  64,  79,  82,  76,
   36,  39,  35,  38,  36,  38,  42,  40,
   26,  21,  21,  19,  24,  21,  33,  28,
    8,   1,   9,   2,   4,   2,  12,   2,
    1,  11,   2,   3,   0,  -5,   6,   1,
    0,   0,   0,   0,   0,   0,   0,   0
  }
};

const int distance_bonus[2][N_RANK] = {
  { 0, 2, 3, 7, 18, 38, 47, 0 },
  { 0, 3, 3, 7, 11, 18, 21, 0 },
};

const int doubled_penalty[N_PHASES] = {6, 22};
const int backward_penalty[N_PHASES] = {-2, 2};
const int isolated_penalty[N_PHASES] = {4, 2};

const int mobility[N_PHASES][N_PIECES - 1][32] = {
  {
    {},
    { -1, 20, 31, 38, 47, 55, 61, 69, 74 },
    { -2, 14, 28, 31, 39, 45, 47, 47, 49, 50, 54, 74, 83, 99 },
    { -62, 1, 17, 20, 25, 23, 27, 30, 32, 34, 36, 38, 40, 49, 40 },
    { -136, -29, 7, 11, 12, 13, 17, 16, 21, 21, 21, 20, 19, 19,
        18, 21, 20, 17, 17, 25, 30, 35, 54, 24, 63, 54, 32, 31 },
  }, {
    {},
    { -60, -13, 7, 13, 19, 27, 24, 17, 1 },
    { -30, -10, -4, 8, 15, 23, 24, 26, 31, 26, 29, 15, 24, 6 },
    { -33, 6, 31, 42, 48, 55, 55, 58, 59, 65, 67, 67, 71, 71, 72 },
    { -6, -25, -21, 13, 14, 30, 38, 51, 47, 62, 71, 80, 83, 91,
       97, 100, 102, 103, 98, 102, 99, 97, 90, 98, 95, 94, 104, 92 },
  }
};

const int threats[N_PHASES][N_PIECES - 2][8] = {
  {
    {},
    { -3, -1, 25, 56, 33 },
    {  4, 21,  0, 41, 45 },
    {  1, 20, 21, 10, 66 },
  }, {
    {},
    { 13, 11, 45, 33,  9 },
    { 10, 43, 27, 32, 47 },
    { 14, 27, 41,  5, 26 },
  }
};

const int threat_king[N_PHASES] = {8, 24};
const int threat_protected_pawn[N_PHASES] = {57, 51};
const int threat_protected_pawn_push[N_PHASES] = {15, 11};

const int rook_file_bonus[N_PHASES][2] = { {14, 29}, {3, 6} };
const int bishop_pair[N_PHASES] = {41, 41};
const int pawn_mobility[N_PHASES] = {1, 7};

void init_distance()
{
  int i, j;

  for (i = 0; i < BOARD_SIZE; i ++)
    for (j = 0; j < BOARD_SIZE; j ++)
      distance[i][j] = _min(_max(_abs(_rank(i) - _rank(j)), _abs(_file(i) - _file(j))), 5);
}

void init_pst()
{
  int piece, sq, v;

  memset(pst_mid, 0, sizeof(pst_mid));
  memset(pst_end, 0, sizeof(pst_end));

  for (piece = 0; piece < N_PIECES; piece ++)
    for (sq = 0; sq < BOARD_SIZE; sq ++)
    {
      v = piece_value[piece] + pst_mid_base[piece][sq];
      pst_mid[piece][sq] = v;
      pst_mid[piece | CHANGE_SIDE][sq ^ 56] = v;

      v = piece_value[piece] + pst_end_base[piece][sq];
      pst_end[piece][sq] = v;
      pst_end[piece | CHANGE_SIDE][sq ^ 56] = v;
    }
}
