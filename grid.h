/**
 * @file grid.h
 * @author TEAM PINE
 * @brief: exports functionality for the grid module.
 * The grid module holds map data and offers
 * interface functions for checking and updating the data as necessary.
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
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "file.h"         /* file operations */
#include "message.h"      /* message operations */

typedef struct grid {
  char** g;
  int rows;
  int cols;
} grid_t;

typedef struct player player_t;
typedef struct game gamestate_t;


/**
 * @brief: function to initialize a grid instance to hold map data.
 * This function allocates memory, which the caller
 * must later free by callind grid_delete()
 * 
 * Inputs:
 * @param mapfile: FILE pointer to the source file containing
 * the map data. It's expected to be open for reading.
 * 
 * Returns:
 * @return grid_t*: pointer to a grid instance containing the map data.
 */
grid_t* grid_init(FILE* mapfile);


/**
 * @brief: function to initialize a grid for a new player.
 * Rather than loading a player from FILE, 
 * this function takes in a pointer to a master grid
 * and copies needed information from that grid instance.
 * 
 * Inputs: 
 * @param masterGrid 
 * 
 * Returns:
 * @return grid_t*: pointer to grid struct
 */
grid_t* grid_initForPlayer(grid_t* masterGrid);


/**
 * @brief: function to delete a grid instance 
 * and free all allocated memory.
 * The grid struct should have been created
 * via a call to either grid_init()
 * or grid_initForPlayer()
 * 
 * Inputs:
 * @param grid: the grid instance
 * 
 * Returns: None.
 */
void grid_delete(grid_t* grid);


/**
 * @brief: function to get the number of rows in a grid instance.
 * 
 * Inputs:
 * @param grid: pointer to a grid instance
 * 
 * Returns:
 * @return int: number of rows.
 * NOTE: we return -1 as a flag for a NULL grid.
 * The caller may check that conditions 
 * in the return value to detect errors.
 */
int grid_getRows(grid_t* grid);


/**
 * @brief: function to get the number of columns in a grid instance.
 * 
 * Inputs:
 * @param grid: pointer to a grid instance
 * 
 * Returns:
 * @return int: number of columns.
 * NOTE: we return -1 as a flag for a NULL grid.
 * The caller may check that conditions 
 * in the return value to detect errors.
 */
int grid_getColumns(grid_t* grid);


/**
 * @brief: function to get the raw representation
 * of the map from a grid struct.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct.
 * @return char**: an array of sentences (2D char array)
 * NOTE: we do not allocate memory for the return value.
 * The caller may NOT free the pointer returned herein.
 */
char** grid_getGrid(grid_t* grid);


/**
 * @brief: function to check if a given point in the grid
 * is a player.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct holding map data.
 * @param x: x position. of point to check in the map.
 * @param y: y position of point to check in the map.
 * 
 * Returns:
 * @return true: given point is a wall.
 * @return false: given point is NOT a wall.
 */
bool grid_isWall(grid_t* grid, int x, int y);


/**
 * @brief: function to check if a given point in the grid
 * is a player.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct holding map data.
 * @param x: x position. of point to check in the map.
 * @param y: y position of point to check in the map.
 * 
 * Returns:
 * @return true: there's a player at the given point.
 * @return false: there's no player at the given point.
 */
bool grid_isPlayer(grid_t* grid, int x, int y);


/**
 * @brief: function to check if a given point in the grid
 * contains gold.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct holding map data.
 * @param x: x position. of point to check in the map.
 * @param y: y position of point to check in the map.
 * 
 * Returns:
 * @return true: given point contains gold.
 * @return false: given point does NOT contain gold.
 */
bool grid_isGold(grid_t* grid, int x, int y);


/**
 * @brief: function to check if a given point in the grid
 * is a passage.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct holdint map data.
 * @param x: x position of point to check in the map.
 * @param y: y position of point to check in the map.
 * 
 * Returns:
 * @return true: given point is a passage.
 * @return false: given point is NOT a passage.
 */
bool grid_isPassage(grid_t* grid, int x, int y);


/**
 * @brief: function to check if a given point in the grid
 * is a playable space.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct holdint map data.
 * @param x: x position. of point to check in the map.
 * @param y: y position of point to check in the map.
 * 
 * Returns:
 * @return true: given point is a wall.
 * @return false: given point is NOT a wall.
 */
bool grid_isSpace(grid_t* grid, int x, int y);


/**
 * @brief: function to check if a player can move in some
 * direction given a key.
 * 
 * Inputs:
 * @param grid: pointer to a grid struct representing the map.
 * @param player: pointer to a player trying to move.
 * @param k: action character that triggered the call.
 * 
 * Returns:
 * @return true: the next point in given direction is a valid move.
 * @return false: the next point in given direction NOT a valid move.
 */
bool grid_canMove(grid_t* master, player_t* player, char k);

void grid_calculateVisibility(grid_t* Grid, player_t* player);

/**
 * @brief: function to convert master grid to string.
 * This function takes in the grid and the gamestate
 * for the current instance of the game.
 * This is important because information from the gamestate
 * is needed to locate and place other players in the
 * stringified representation of the master grid.
 * 
 * Inputs:
 * @param masterGrid: pointer to the grid instance
 * representing the master grid for the current session of the game.
 * @param gamestate: pointer to the gamestate instance
 * for the current session of the game.
 * 
 * Returns:
 * @return char*: a string representation of the master grid,
 * with all the player information filled in.
 * 
 * NOTE: This function allocates memory for the returned string
 * via calls to another function. The caller of this function
 * must later call free() on the returned pointer.
 * 
 */
char* grid_masterGridToString(grid_t* masterGrid, gamestate_t* state);


/**
 * @brief: function to generate an identical copy of a grid instance.
 * 
 * Inputs: 
 * @param original_grid 
 * 
 * Returns:
 * @return grid_t*: a pointer to a grid struct 
 * identical to the one that was passed in as an argument.
 * 
 * NOTE: this function allocates memory for the copy of the grid
 * through calls to other functions. The caller of this function
 * must later free that pointer by calling grid_delete().
 */
grid_t* grid_copy(grid_t* original_grid);


/**
 * @brief: function to generate a string representation
 * of a grid instance.
 * 
 * Inputs:
 * @param state: pointer to gamestate for the current game instance
 * @param grid: pointer to the grid to be stringified
 * 
 * Returns:
 * @return char*: a string representation of the grid.
 */
char* grid_toString(gamestate_t* state, grid_t* grid);

#endif /* __GRID_H */