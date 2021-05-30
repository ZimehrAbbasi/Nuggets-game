/**
 * @file gold.h
 * @author TEAM PINE
 * @brief exports functionality for gold module
 * @version 0.1
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __GOLD_H
#define __GOLD_H

/* standard libraries */
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>

#include "grid.h"       /* grid module */
#include "player.h"     /* player module */

typedef struct gold {
  int* goldCounter;
  int numPiles;
  int index;
} gold_t;

gold_t* gold_init(int numPiles);

void gold_delete(gold_t* gold);

void gold_assign(gold_t* gold, player_t* player);

void gold_distribute(grid_t* grid, gold_t* gold);

#endif /* __GOLD_H */