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


/**
 * @brief: function to initialize the state of gold
 * in a game instance and scatter gold in the map.
 * 
 * Inputs:
 * @param numPiles: The total desired number of piles of gold to assign.
 * 
 * Returns:
 * @return gold_t*: pointer to a struct holding gold information.
 */
gold_t* gold_init(int numPiles);


/**
 * @brief: function to delete gold information when needed
 * (e.g. at end of the game.)
 * 
 * Inputs: 
 * @param gold 
 * 
 * Returns: None
 * 
 * NOTE: The gold data must have been created using gold_init()
 * as this function frees the memory in expectation with what
 * memory gold_init() allocates.
 */
void gold_delete(gold_t* gold);


/**
 * @brief: function to assign a pile of gold to a given player.
 * 
 * Inputs: 
 * @param gold: pointer to a gold struct created using gold_init()
 * @param player: pointer to a player struct 
 * such as the one implemented in player.c and player.h
 * 
 * Returns: None.
 */
void gold_assign(gold_t* gold, player_t* player);

/**
 * @brief: function to scatter gold in a map,
 * given a map (grid struct) and a gold strict with relevant details.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct wherein the map data is stored.
 * @param gold: pointer to gold struct holding information
 * such as the number of piles to scatter in the map.
 * 
 * Returns: None.
 */
void gold_distribute(grid_t* grid, gold_t* gold);

#endif /* __GOLD_H */