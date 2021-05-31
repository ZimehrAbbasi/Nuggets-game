/**
 * @file grid.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __GRID_H
#define __GRID_H

/* standard libs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct grid {
  char** g;
  int rows;
  int cols;
} grid_t;

typedef struct player player_t;

grid_t* grid_init(FILE* mapfile);

grid_t* grid_initForPlayer(grid_t* masterGrid);

char* grid_toString(grid_t* Grid);

void grid_delete(grid_t* grid);

int grid_getRows(grid_t* grid);

int grid_getRows(grid_t* grid);

char** grid_getGrid(grid_t* grid);

bool grid_isWall(grid_t* grid, int x, int y);

bool grid_isPlayer(grid_t* grid, int x, int y);

bool grid_isGold(grid_t* grid, int x, int y);

bool grid_isPassage(grid_t* grid, int x, int y);

bool grid_isSpace(grid_t* grid, int x, int y);

bool grid_canMove(grid_t* master, player_t* player, char k);

#endif /* __GRID_H */