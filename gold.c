/**
 * @file gold.c
 * @author TEAM PINE
 * @brief implements functionality for the gold module.
 * The gold module tracks the total gold in the game and
 * assigns gold to players as they collect it.
 * It also decrements the gold as appropriate.
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

#include "grid.h"       /* grid module */
#include "player.h"     /* player module */
#include "gold.h"       /* self */


/******** static function prototypes *******/
static int randomInt(int lower, int upper);

/******** module constants *******/
static const int TOTALGOLD = 250;     /* the total gold assigned at start of the game. */

/************** Exported functions ***************/

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
gold_t*
gold_init(int numPiles)
{
  /* cannot allocate negative number of gold piles */
  if (numPiles >= 0) {

    /* allocate memory for gold struct */
    gold_t* gold = malloc(sizeof(gold_t));

    /* save number of piles */
    gold->numPiles = numPiles;

    /* allocate memory for gold counter array */
    gold->goldCounter = calloc(numPiles, sizeof(int));

    int remainder = TOTALGOLD;                          /* track remaining gold */ 
    for (int i = 0; i < numPiles - 1; i++) {            /* allocate random counts for numPiles-1 piles */
      int upperBound = (int) (remainder / (numPiles - i));
      gold->goldCounter[i] = randomInt(1, upperBound);
      remainder -= gold->goldCounter[i];
    }
    /* allocate remainder to last pile (to add up to 250) */
    gold->goldCounter[numPiles - 1] = remainder;

    /* initialize index in piles to ZERO */
    gold->index = 0;

    /* return created struct */
    return gold;
  }
  fprintf(stderr, "Attempt to allocate a negative number of gold piles. Stop.\n");
  return NULL; 
}


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
void
gold_delete(gold_t* gold)
{
  /* if struct is not null, free it's counter then free the struct. */
  if (gold != NULL) {
    free(gold->goldCounter);
    free(gold);
  }
}

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
void
gold_assign(gold_t* gold, player_t* player)
{
  /* if both player and gold are not null,
     and there are still piles of gold remaining in the game,
     get the next pile of gold and assign to player. */
  if (gold != NULL && player != NULL) {
    if (gold->index < gold->numPiles) {
      player_addGold(player, gold->goldCounter[gold->index++]);
    }
  }
}


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
void
gold_distribute(grid_t* grid, gold_t* gold)
{
  /* get raw representation of map in the grid. */
  char** map = grid_getGrid(grid);

  /* initialize counter for number of piles assigned. */
  int i = 0;

  /* repeat until desired number of piles is assigned... */
  while (i < gold->numPiles) {

    /* generate random coordinates */
    int x = randomInt(1, grid_getColumns(grid));
    int y = randomInt(1, grid_getRows(grid));

    /* if random coordinates are a valid point,
       place a line there in the map.
       if not valid, repeat. */
    if (map[y][x] == '.') {
      map[y][x] = '*';
      i++;
    }
  }
}


/**************** Static Functions ******************/

/**
 * @brief: Function to generate a number within a range.
 * calls rand() to generate a random number 
 * then bounds it to expectate range using mod operator.
 * This is handy because rand() doesn't take any arguments
 * and generates unbounded values
 * 
 * Inputs:
 * @param lower: lower bound, inclusive
 * @param upper: upper bound, inclusive
 * 
 * Returns:
 * @return int: a random value between the lower and upper bound.
 * 
 * NOTE: This function returns -1 as an error flag for invalid bounds.
 * If called in a context that may result in incorrect bounds, 
 * The caller may check if the return value is -1
 * To detect errors. 
 */
static int
randomInt(int lower, int upper)
{
  /* can only generate a calue within valid bounds... */
  if (lower < upper) {

    /* initialize number to lower bound */
    int num = lower;

    /* generate random number */
    int randomNumber = rand();

    /* add the mod of the random number and (upper - lower)
       add value to lower bound */
    num += randomNumber % (upper - lower);
    return num;
  }

  /* if bounds are invalid, print error message, return -1 */
  fprintf(stderr, "Attempt to generate a number with invalid bounds. Stop.\n");
  return -1;
}