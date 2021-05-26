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

typedef struct grid grid_t;

grid_t* grid_init(FILE* mapfile, int rows, int cols);

char* grid_toString(grid_t* Grid);

void grid_delete(grid_t* grid);

int grid_getRows(grid_t* grid);

int grid_getRows(grid_t* grid);

char** grid_getGrid(grid_t* grid);

#endif /* __GRID_H */