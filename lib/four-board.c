#include "four-board.h"

struct four_board {
  size_t width;
  size_t height;
  short *board; //length w * h
};

fboard *board_new(size_t w, size_t h) {
  fboard *ret = malloc(sizeof(fboard));
  memCheck((void *)ret);
  ret->width = w;
  ret->height = h;
  ret->board = calloc(sizeof(short), w * h);
  memCheck((void *)ret->board);
  return ret;
}

short play(fboard *b, int x, short player) {
  int a;
  for(a = b->height - 1; a >= 0; a--) {
    if(b->board[getIndex(b, x, a)] == 0) {
      b->board[getIndex(b, x, a)] = player;
      break; 
    }    
  }
  if(a == -1)
    return -1;
  return checkWin(b, 0);
}

bool isFull(fboard *b) {
  for(unsigned int x = 0; x < b->width; x++)
    for(unsigned int y = 0; y < b->height; y++)
      if(b->board[getIndex(b, x, y)] == 0)
        return false;
  return true;
}

bool checkWinAt(fboard *b, int x, int y, int xChange, int yChange, int count) {
  if(x + xChange >= (int)b->width || y + yChange >= (int)b->height)
    return false;
  if(x + xChange < 0 || y + yChange < 0)
    return false;
  int i = getIndex(b, x, y);
  int i_new = getIndex(b, x + xChange, y + yChange);
  if(b->board[i] == 0 || b->board[i] != b->board[i_new])
    return false;
  if(count == 2)
    return true;
  return checkWinAt(b, x + xChange, y + yChange, xChange, yChange, count + 1);
}

short checkWin(fboard *b, size_t i) {
  if(i >= b->width * b->height)
    return 0;
  int x = getCol(b, i);
  int y = getRow(b, i);
  if(checkWinAt(b, x, y, 1, 0, 0) ||
    checkWinAt(b, x, y, -1, 0, 0) ||
    checkWinAt(b, x, y, 0, 1, 0) ||
    checkWinAt(b, x, y, 0, -1, 0) ||
    checkWinAt(b, x, y, 1, 1, 0) ||
    checkWinAt(b, x, y, -1, -1, 0) ||
    checkWinAt(b, x, y, -1, 1, 0) ||
    checkWinAt(b, x, y, 1, -1, 0))
    return b->board[i];
  return checkWin(b, i + 1);
}

size_t getWidth(fboard *b) {
  return b->width;
}

size_t getHeight(fboard *b) {
  return b->height;
}

short getState(fboard *b, int x, int y) {
  return b->board[getIndex(b, x, y)];
}

int getCol(fboard *b, size_t i) {
  assert(i < b->height * b->width);
  return i % b->width;
}

int getRow(fboard *b, size_t i) {
  assert(i < b->height * b->width);
  return i / b->width;
}

int getIndex(fboard *b, int x, int y) {
  assert(y < (int)b->height && x < (int)b->width);
  assert(y >= 0 && x >= 0);
  return y * b->width + x; 
} 

void printBoard(fboard *b) {
  for(int a = 0; a < (int)(b->width * b->height); a++)
    if(a % b->width == 0)
      printf("\n%d ", b->board[a]);
    else
      printf("%d ", b->board[a]);
}
