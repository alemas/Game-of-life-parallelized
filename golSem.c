// compile with gcc gol.c -o gol -lncurses -fopenmp
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//#include "ncurses.h"

#define TIME_TEST 1
#define ITERATIONS_LIMIT 100
#ifdef TIME_TEST
FILE * timeLog = NULL;
#endif

int getValue(int x, int y, int * grid, unsigned int width, unsigned int height);
void setValue(int value, int x, int y, int * grid, int * needsCheck, unsigned int width, unsigned int height);
void clearGrid(int * grid, unsigned int size, int value);
void update(int * grid, int * needsCheck, unsigned int width, unsigned int height);
int getNeighbours(unsigned int x, unsigned int y, int * grid, unsigned int width, unsigned int height);
void randomGrid(int * grid, int * needsCheck, unsigned int size);

int main(void) {
  // Inicia o gerador de números aleatórios sempre com a mesma seed para que o resultado seja sempre o mesmo
  srand( 42 );

  // Inicia o array da simulação
  unsigned int maxX = 2000;
  unsigned int maxY = 2000;
  unsigned int bottomY = maxY;
  maxY -= 1;

  int *grid = malloc(maxX * maxY * sizeof(int));
  int *needsCheck = malloc(maxX * maxY * sizeof(int));
  randomGrid(grid, needsCheck, maxX * maxY);

  int currentIteration = 0;
  while(currentIteration < ITERATIONS_LIMIT) {
    update(grid, needsCheck, maxX, maxY);
    currentIteration++;
  }
}

// Método para buscar uma posição no array usando coordenadas x e y
int getValue(int x, int y, int * grid, unsigned int width, unsigned int height) {
  if(x <= -1) {
    x = width - 1;
  }
  else if(x >= width) {
    x = 0;
  }
  if(y <= -1) {
    y = height - 1;
  }
  else if(y >= height) {
    y = 0;
  }
  return grid[x +(width*y)];
}

// Método para definir o valor de uma posição no array usando coordenadas x e y
void setValue(int value, int x, int y, int * grid, int * needsCheck, unsigned int width, unsigned int height) {
  if(x <= -1) {
    x = width - 1;
  }
  else if(x >= width) {
    x = 0;
  }
  if(y <= -1) {
    y = height - 1;
  }
  else if(y >= height) {
    y = 0;
  }
  grid[x + (width*y)] = value;

  if(needsCheck != NULL) {
    // give the option of sending in null for big batches of changes like clears
    // this way we dont repeatedly set things to a value
    // also if a value hasnt changed
    needsCheck[x + (width*y)] = 1; // we have changed
    // next do our moores neighbours
    int i;
    for(i = x-1; i <= x+1; i ++) {
      int j;
      for(j = y-1; j<= y+1; j ++) {
        setValue(1, i, j, needsCheck, NULL, width, height);
      }
    }

  }
}

void clearGrid(int * grid, unsigned int size, int value) {
  unsigned int i;
  for(i = 0; i < size; i ++) {
    grid[i] = value;
  }
}


void update(int * grid, int * needsCheck, unsigned int width, unsigned int height) {
  int *newGrid = malloc(width * height * sizeof(int));
  int *newNeedsCheck = malloc(width * height * sizeof(int));
  clearGrid(newNeedsCheck, width * height, 0);
  unsigned int x, y;
  
  for(x = 0; x < width; x ++) {
    for(y = 0; y < height; y ++) {
      int state = getValue(x, y, grid, width, height);
      if (getValue(x, y, needsCheck, width, height) == 0) {
        setValue(state, x, y, newGrid, NULL, width, height); 
        continue;
      }
      int neighbours = getNeighbours(x, y, grid, width, height);
      if(state == 0) {
        // dead
        if(neighbours == 3) {
          setValue(1, x, y, newGrid, newNeedsCheck, width, height);
        }
        else {
          // no change
          setValue(0, x, y, newGrid, NULL, width, height);
        }
      }
      else if(state == 1) {
        // alive
        if(neighbours < 2 || neighbours > 3) {
          setValue(0, x, y, newGrid, newNeedsCheck, width, height);
        }
        else {
          // no change
          setValue(1, x, y, newGrid, NULL, width, height);
        }
      }
    }
  }

  // replace old grid
  unsigned int i;
  for(i = 0; i < width*height; i ++) {
    grid[i] = newGrid[i];
    needsCheck[i] = newNeedsCheck[i];
  }
}

int getNeighbours(unsigned int x, unsigned int y, int * grid, unsigned int width, unsigned int height) {
  int total = 0;
  total += getValue(x-1, y, grid, width, height);
  total += getValue(x-1, y-1, grid, width, height);
  total += getValue(x-1, y+1, grid, width, height);
  total += getValue(x, y-1, grid, width, height);
  total += getValue(x, y+1, grid, width, height);
  total += getValue(x+1, y-1, grid, width, height);
  total += getValue(x+1, y+1, grid, width, height);
  total += getValue(x+1, y, grid, width, height);
  return total;
}

void randomGrid(int * grid, int * needsCheck, unsigned int size) {
  unsigned int i;
  for(i = 0; i < size; i ++) {
    grid[i] = rand() % 10 + 1 < 5 ? 0 : 1;
    needsCheck[i] = 1;
  }

}