#include "utils.h"
/* P1, P2 > 0 */
#define P1 1
#define P2 2


#ifndef _FOUR_BOARD_H_
#define _FOUR_BOARD_H_

struct four_board;
typedef struct four_board fboard;

fboard *board_new(size_t width, size_t height);
/* An fboard is an array of states. Each space in the board can
 * be occupied by either player (values P1, P2 respectively) or 
 * it is empty, in which case it is 0 */

short play(fboard *b, int x, short player);

bool checkWinAt(fboard *b, int x, int y, int xChange, int yChange, int count);

bool isFull(fboard *b);

short checkWin(fboard *b, size_t i);

short getState(fboard *b, int x, int y);

int getIndex(fboard *b, int x, int y);

int getCol(fboard *b, size_t i);

int getRow(fboard *b, size_t i);

size_t getWidth(fboard *b);

size_t getHeight(fboard *b);

void printBoard(fboard *b);
#endif
