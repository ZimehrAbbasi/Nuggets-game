/**
 * @file grid.c
 * @author TEAM PINE
 * @brief: implements functionality for the grid module.
 * The grid module holds map data and offers
 * interface functions for checking and updating the data as necessary.
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "file.h"         /* file operations */
#include "message.h"      /* message operations */
#include "player.h"       /* player module */
#include "gamestate.h"    /* gamestate module */
#include "grid.h"         /* self */


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
grid_t* grid_init(FILE* mapfile) {

  if (mapfile != NULL) {
    
    /* make sure we return to start of file in case of prior reads */
    rewind(mapfile);

    int rows = file_numLines(mapfile);
    char* firstLine = file_readLine(mapfile);
    int cols = strlen(firstLine);
    free(firstLine);

    /* reset */
    rewind(mapfile);
    
    /* check validity of map */
    if (rows == 0 || cols == 0) {
      return NULL;
    }

    grid_t* grid  = malloc(sizeof(grid_t));
    if (grid == NULL) {
      return NULL; 
    }

    /* save rows, columns */
    grid->rows = rows;
    grid->cols = cols;

    /* create map representation */
    grid->g = malloc((rows+1) * sizeof(char*));

    /* allocate rows */
    char** temp = grid->g;
    for(int i=0; i<rows; i++){
      temp[i] = malloc(cols+1);
    }

    /* copy lines from file as rows */
    int j = 0;
    char* line;
    while( (line = file_readLine(mapfile)) != NULL){
      strcpy(temp[j++], line);
      free(line);
    }
    /* return grid */
    return grid;
  }

  /* in case pointer to map file is NULL */
  fprintf(stderr, "Attempt to create grid with a NULL name of map file. Stop.\n");
  return NULL;
}


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
grid_t*
grid_initForPlayer(grid_t* masterGrid)
{
  if (masterGrid != NULL) {
    grid_t* grid  = malloc(sizeof(grid_t));
    if (grid == NULL) {
      return NULL; 
    }

    /* save rows, columns */
    grid->rows = masterGrid->rows;
    grid->cols = masterGrid->cols;

    /* create map representation */
    grid->g = malloc((masterGrid->rows + 1) * sizeof(char*));

    /* allocate rows */
    for(int i=0; i<masterGrid->rows; i++){
      grid->g[i] = malloc(masterGrid->cols);
    }
    /* fill player grid with spaces as holders */
    for (int y=0; y<grid->rows; y++) {
      for (int x=0; x<grid->cols; x++) {
        grid->g[y][x] = ' ';
      }
    }

    /* return grid */
    return grid;
  }

  /* if master grid is NULL, print error message and return NULL */
  fprintf(stderr, "Error: cannot init grid to match a NULL grid. Stop.\n");
  return NULL;
}


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
char* 
grid_toString(gamestate_t* state, grid_t* grid)
{
  if (state == NULL || grid == NULL) {
    return NULL;
  }
  // Get a copy of the master grid
  grid_t* copy = grid_copy(state->masterGrid);

  // Loop through and add player chars for associated points into copy of grid
  player_t** allPlayers = state->players;
  for(int i = 0; i < state->players_seen; i++){
    // Update copy of grid with player letters
    int currentPlayerX = allPlayers[i]->x;
    int currentPlayerY = allPlayers[i]->y;

    copy->g[currentPlayerY][currentPlayerX] = allPlayers[i]->letter;
  }

  char* stringifiedGrid = calloc(1, ((copy->rows)*sizeof(char*)) * copy->cols);
  char** map = copy->g;
  for(int x = 0; x < copy->rows; x++){
    strcat(stringifiedGrid, map[x]);
    if (x != copy->rows-1) {
      strcat(stringifiedGrid, "\n");
    }  
  }

  // Free copy of grid
  grid_delete(copy);

  // Return stringified result
  return stringifiedGrid;
}


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
grid_t*
grid_copy(grid_t* originalGrid)
{
  // Create pointer to new grid object with correct dimensions
  grid_t* copy = grid_initForPlayer(originalGrid);

  // Loop over every line in orig. grid and copy into new grid
  for(int y = 0; y < copy->rows; y++){
    // Copy line
    strcpy(copy->g[y], originalGrid->g[y]);
  }

  return copy;
}

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
char* 
grid_masterGridToString(grid_t* masterGrid, gamestate_t* state)
{

  /* only doable if both pointers are not NULL */
  if (masterGrid != NULL && state != NULL) {

    /* create string version of grid */
    char* map = grid_toString(state, masterGrid);

    /* get number of columns */
    int cols = grid_getColumns(masterGrid);

    /* get players in game */
    player_t** players = gamestate_getPlayers(state);

    /* loop over all possible players */
    for (int i = 0; i<26; i++) {

      /* if player is not NULL,
         get position and add them to map. */
      if (players[i] != NULL) {
        int x = player_getX(players[i]);
        int y = player_getY(players[i]);
        char letter = player_getLetter(players[i]);

        /* position = (length of previous rows) + 
           (total "\n" appended ) + 
           pos in current row */ 
        int pos = cols*(y) + y + x;            

        /* set position in string to represent a player */
        map[pos] = letter;
      }
    }

    /* return constructed representation */
    return map;
  }

  /* if either pointer is NULL, print error message
     and return NULL */
  fprintf(stderr, "Attempt to convert NULL master grid or gamestate. Stop.\n");
  return NULL;
  
}


/**
 * @brief: function to check if a given point in the grid
 * is a player.
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
bool
grid_isWall(grid_t* grid, int x, int y)
{
  if (grid != NULL) {
    char **master = grid->g;
    return (master[y][x] == '|' || master[y][x] == '-' || 
            master[y][x] == '+' || master[y][x] == ' ');
  }

  /* if grid is NULL,
     print error and return false. */
  fprintf(stderr, "Error: attempt to check position in NULL grid. Stop.\n");
  return false;
}


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
bool
grid_isPlayer(grid_t* grid, int x, int y)
{
  if (grid != NULL) {
    char **master = grid->g;
    return isalpha(master[y][x]);
  }

  /* if grid is NULL,
     print error and return false. */
  fprintf(stderr, "Error: attempt to check position in NULL grid. Stop.\n");
  return false;
}


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
bool
grid_isGold(grid_t* grid, int x, int y)
{

  if (grid != NULL) {
    char **master = grid->g;
    return master[y][x] == '*';
  }

  /* if grid is NULL,
     print error and return false. */
  fprintf(stderr, "Error: attempt to check position in NULL grid. Stop.\n");
  return false;
}


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
bool
grid_isPassage(grid_t* grid, int x, int y)
{
  
  if (grid != NULL) {
    char** master = grid->g;
    return master[y][x] == '#';
  }
  return -1;
}


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
bool
grid_isSpace(grid_t* grid, int x, int y) {
  char **master = grid->g;
  return master[y][x] == '.';
}

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
bool 
grid_canMove(grid_t* grid, player_t* player, char k)
{
  char letter = k;
  switch (letter) {
  case 'l': 
    if(!grid_isWall(grid, player->x+1, player->y)){
      return true;
    }
    break;
  case 'h': 
    if(!grid_isWall(grid, player->x-1, player->y)){
      return true;
    }
    break;
  case 'k': 
    if(!grid_isWall(grid, player->x, player->y+1)){
      return true;
    }
    break;
  case 'j': 
    if(!grid_isWall(grid, player->x, player->y-1)){
      return true;
    }
    break;
  case 'u': 
    if(!grid_isWall(grid, player->x+1, player->y+1)){
      return true;
    }
    break;
  case 'y': 
    if(!grid_isWall(grid, player->x-1, player->y+1)){
      return true;
    }
    break;
  case 'b': 
    if(!grid_isWall(grid, player->x-1, player->y-1)){
      return true;
    }
    break;
  case 'n': 
    if(!grid_isWall(grid, player->x+1, player->y-1)){
      return true;
    }
    break;
  default:
    return false;
  }
  return false;
}

/**
 * @brief: static function to calculate the slope
 * of the direct line of vision between two points
 * in the grid.
 * 
 * @param x1: coordinate x of first point.
 * @param y1: coordinate y of first point. 
 * @param x2: coordinate x of second point. 
 * @param y2: coordinate y of second point. 
 * @return double: value of slope of 
 * direct line of sight
 * from point one to point two
 */
static double calculate_slope(int x1, int y1, int x2, int y2){

  if (y1 == y2) {
    return DBL_MAX;
  }

  return ((double)(x1 - x2))/((double)(y1 - y2));
}

static double func(int x, int y, double slope){
    return ((slope * (double)x) + (double)y);
}

static int max(int num1, int num2){
    return num1 > num2 ? num1 : num2;
}

static int min(int num1, int num2){
    return num1 > num2 ? num2 : num1;
}

// void grid_calculateVisibility(grid_t* Grid, player_t* player){
    
//     printf("\nPlayer X: %d\n PlayerY: %d\n", player->x, player->y);
    
//     char **master_grid = Grid->g;
//     char **player_grid = player->grid->g;
//     double slope;
//     double x_pred, y_pred;
//     int upper, lower;
//     bool visibility;
// 	master_grid[player->y][player->x] = 'A';
// 	for(int y = 0; y < Grid->rows; y++){
// 		for(int x = 0; x < Grid->cols; x++){
// 			printf("%c", master_grid[y][x]);
// 		}
// 		printf("\n");
// 	}
	
// 	for(int y = 0; y < Grid->rows; y++){
//     	for(int x = 0; x < Grid->cols; x++){
// 			x = 10;
// 			y = 1;
//             if(master_grid[y][x] == ' '){
//               continue;
//             }
//             slope = calculate_slope(x, y, player->x, player->y);

// 			if(player->x == x){
// 				int x1 = player->x;
// 				while(!grid_isWall()){
					
// 				}
// 			}

// 			printf("%f\n", slope);
//             visibility = true;
//             if(abs(player->y - y) > abs(player->x - x)){
// 				int start = min(y, player->y);
//                 for(int y1 = 0; y1 < max(y, player->y)-start; y1++){

// 					if(x < player->x){
// 						x_pred = 1/slope * y1 + x;
// 					}else{
// 						x_pred = -1/slope * y1 + player->x;;
// 					}
// 					int pred_y = start - y1;
//                     if(x_pred < 0 || x_pred > Grid->cols || pred_y < 0 || pred_y > Grid->rows){
//                       continue;
//                     }
//                     upper = (int)ceil(x_pred);
//                     lower = (int)floor(x_pred);
//                     if(grid_isWall(Grid, upper, pred_y) || grid_isWall(Grid, lower, pred_y)){
//                       	visibility = false;
//                       	break; 
//                     }
//                 }
//             }else{
// 				int start = min(x, player->x);
//                 for(int x1 = 0; x1 < max(x, player->x)-start; x1++){
					
// 					if(y < player->y){
// 						y_pred = -slope * x1 + y;
// 					}else{
// 						y_pred = slope * x1 + player->y;
// 					}
//                     int pred_x = start - x1;
//                     if(y_pred < 0 || y_pred > Grid->rows || pred_x < 0 || pred_x > Grid->cols){
//                       	continue;
//                     }
//                     upper = (int)ceil(y_pred);
//                     lower = (int)floor(y_pred);
					
//                     if(grid_isWall(Grid, pred_x, upper) || grid_isWall(Grid, pred_x, lower)){
//                       	visibility = false;
//                       	break; 
//                     }
//                 }
//             }

//             if(visibility){
//                 player_grid[y][x] = master_grid[y][x];
//             }else{
//                 if(grid_isGold(Grid, x, y)){
//                     player_grid[y][x] = '.';
//                 }
//             }

// 			exit(1);
//         }
//     }

// }

void grid_movePlayer(gamestate_t* gameState, player_t* player, int x, int y){
    grid_t* master = gameState->masterGrid;
    gold_t* gameGold = gameState->gameGold;
	char** player_grid = player->grid->g;
	char** master_grid = master->g;
	int* gold_array = gameGold->goldCounter;

    if (grid_isGold(master, x, y)){
		player_grid[player->y][player->x] = '.';
		master_grid[player->y][player->x] = '.';

        player->x = x;
		player->y = y;
        
        player->gold = gold_array[gameGold->index];
        // gameGold->goldremaining -= gold_array[gameGold->index];
        gameGold->index += 1;
        
        master_grid[y][x] = player->letter;
		player_grid[y][x] = player->letter;
        
        
    }else if(grid_isPlayer(master, x, y)){

		player_t **players = gameState->players;
        for(int i = 0; i < 26;i++){
			player_t* otherPlayer = players[i];
			if (otherPlayer->x == x && otherPlayer->y == y){
                otherPlayer->x = player->x;
                otherPlayer->y = player->y;
                char** other_grid = otherPlayer->grid->g;
				other_grid[otherPlayer->y][otherPlayer->x] = otherPlayer->letter;
				master_grid[otherPlayer->y][otherPlayer->x] = otherPlayer->letter;
				break;
			}
		}
        
        player->x = x;
		player->y = y;
        master_grid[y][x] = player->letter;
		player_grid[y][x] = player->letter;

	}else{
        player_grid[player->y][player->x] = '.';
		master_grid[player->y][player->x] = '.';

        player->x = x;
		player->y = y;

		master_grid[y][x] = player->letter;
		player_grid[y][x] = player->letter;
	}
}

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
void
grid_delete(grid_t* grid) {
  if (grid != NULL) {
    for (int y=0; y<grid->rows; y++) {
      free(grid->g[y]);
    }
    free(grid->g);
    free(grid);
  }
}


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
int
grid_getRows(grid_t* grid)
{
  /* cannot get number of rows
     from NULL grid */
  if (grid != NULL) {
    return grid->rows;
  }
  fprintf(stderr, "Grid is NULL. Stop.\n");
  return -1;
}


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
int
grid_getColumns(grid_t* grid)
{
  /* cannot get number of columns
     from NULL grid */
  if (grid != NULL) {
    return grid->cols;
  }
  fprintf(stderr, "Grid is NULL. Stop.\n");
  return -1;
}

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
char**
grid_getGrid(grid_t* grid)
{
  /* cannot get raw representation
     from NULL grid */
  if (grid != NULL) {
    return grid->g;
  }
  /* print error message in the case of NULL pointer */
  fprintf(stderr, "Attempt to get grid representation in a NULL grid. Stop.\n");
  return NULL;
}