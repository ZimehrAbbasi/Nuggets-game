/**
 * @file gold.c
 * @author TEAM PINE
 * @brief 
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>

#include "gold.h"
#include "grid.h"       /* grid module */
#include "player.h"     /* player module */
/******** static function prototypes *******/
static int randomInt(int lower, int upper);

/* module constants */
static const int TOTALGOLD = 250;

/************** Exported functions ***************/
gold_t*
gold_init(int numPiles)
{
  if (numPiles >= 0) {
    gold_t* gold = malloc(sizeof(gold_t));
    gold->numPiles = numPiles;
    gold->goldCounter = calloc(numPiles, sizeof(int));

    int remainder = TOTALGOLD;
    for (int i = 0; i < numPiles; i++) {
      int upperBound = remainder - (numPiles - i - 1);
      gold->goldCounter[i] = randomInt(1, upperBound);
      remainder -= gold->goldCounter[i];
    }
    gold->index = 0;
    return gold;
  }
  fprintf(stderr, "Attempt to allocate a negative number of gold piles. Stop.\n");
  return NULL; 
}


void
gold_delete(gold_t* gold)
{
  if (gold != NULL) {
    free(gold->goldCounter);
    free(gold);
  }
}


void
gold_assign(gold_t* gold, player_t* player)
{
  if (gold != NULL && player != NULL) {
    if (gold->index < gold->numPiles) {
      player_addGold(player, gold->goldCounter[gold->index++]);
    }
  }
}


void
gold_distribute(grid_t* grid, gold_t* gold)
{
  char** map = grid_getGrid(grid);

  int i = 0;
  while (i < gold->numPiles) {
    int x = randomInt(1, grid_getColumns(grid));
    int y = randomInt(1, grid_getRows(grid));
    if (map[y][x] == '.') {
      map[y][x] = '*';
      i++;
    }
  }
}


/**************** Static Functions ******************/

/**
 * @brief: Function to generate a number within a range.
 * repeatedly calls rand() until value
 * between the lower and upper bound is generated.
 * 
 * @param lower: lower bound, inclusive
 * @param upper: upper bound, inclusive
 * @return int: a random value between the lower and upper bound.
 */
static int
randomInt(int lower, int upper)
{
  if (lower < upper) {
    int num = rand();
    while (num < lower || num > upper) {
      num = rand();
    }
    return num;
  }
  fprintf(stderr, "Attempt to generate a number with invalid bounds. Stop.\n");
  return -1;
}