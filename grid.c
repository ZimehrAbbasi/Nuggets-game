#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "support/file.h"         /* file operations */
#include "support/message.h"      /* message operations */
#include "player.h"       /* player module */
#include "gamestate.h"    /* gamestate module */
#include "grid.h"         /* self */

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
  fprintf(stderr, "Error: cannot init grid to match a NULL grid. Stop. \n");
  return NULL;
}

char* grid_toString(gamestate_t* state, grid_t* grid){
  if (grid == NULL){
    return NULL;
  }
  // Get a copy of the master grid
  grid_t* copy = grid_copy(grid);

  // Loop through and add player chars for associated points into copy of grid
  player_t** allPlayers = state->players;
  for(int i = 0; i < state->players_seen; i++){
    printf("\n\n\n\nPLAYER NAME: %s\n\n\n\n", allPlayers[i]->name);
    // Update copy of grid with player letters
    int currentPlayerX = allPlayers[i]->x;
    int currentPlayerY = allPlayers[i]->y;

    copy->g[currentPlayerY][currentPlayerX] = allPlayers[i]->letter;
  }

  char* stringifiedGrid = malloc(((copy->rows)*sizeof(char*)) * copy->cols);
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

grid_t*
grid_copy(grid_t* original_grid){
  // Create pointer to new grid object with correct dimensions
  grid_t* copy = grid_initForPlayer(original_grid);

  // Loop over every line in orig. grid and copy into new grid
  for(int y = 0; y < copy->rows; y++){
    // Copy line
    strcpy(copy->g[y], original_grid->g[y]);
  }

  return copy;
}

char* 
grid_masterGridToString(grid_t* grid, gamestate_t* gamestate)
{
  char* map = grid_toString(gamestate, grid);
  int cols = grid_getColumns(grid);

  player_t** players = gamestate_getPlayers(gamestate);
  for (int i = 0; i<25; i++) {
    if (players[i] != NULL) {
      int x = player_getX(players[i]);
      int y = player_getY(players[i]);
      char letter = player_getLetter(players[i]);
      int pos = cols*(y) + y + x;            /* (length of previous cols) + ("\n" ) + pos in current row */ 

      map[pos] = letter;
    }
  }
  return map;
}

bool
grid_isWall(grid_t* grid, int x, int y)
{
  char **master = grid->g;
  return ( master[y][x] == '|' || 
           master[y][x] == '-' || 
           master[y][x] == '+' || 
           master[y][x] == ' ');
        // return true;
    // }
    // return false;
}

bool
grid_isPlayer(grid_t* grid, int x, int y)
{
  char **master = grid->g;
  return isalpha(master[y][x]);
    // {
    //     return true;
    // }
    // return false;
}

bool
grid_isGold(grid_t* grid, int x, int y)
{
  char **master = grid->g;
  return master[y][x] == '*';
    // {
    //     return true;
    // }
    // return false;
}

bool
grid_isPassage(grid_t* grid, int x, int y)
{
  if (grid != NULL) {
    char **master = grid->g;
    return master[y][x] == '#';
  }
  return -1;
    // {
    //     return true;
    // }
    // return false;
}

bool
grid_isSpace(grid_t* grid, int x, int y) {
  char **master = grid->g;
  return master[y][x] == '.';
}

bool grid_canMove(grid_t* Grid, player_t* player, char k){
    char letter = k;
    switch (letter) {
    case 'l': 
        if(!grid_isWall(Grid, player->x+1, player->y)){
            return true;
        }
        break;
    case 'h': 
        if(!grid_isWall(Grid, player->x-1, player->y)){
            return true;
        }
        break;
    case 'k': 
        if(!grid_isWall(Grid, player->x, player->y+1)){
            return true;
        }
        break;
    case 'j': 
        if(!grid_isWall(Grid, player->x, player->y-1)){
            return true;
        }
        break;
    case 'u': 
        if(!grid_isWall(Grid, player->x+1, player->y+1)){
            return true;
        }
        break;
    case 'y': 
        if(!grid_isWall(Grid, player->x-1, player->y+1)){
            return true;
        }
        break;
    case 'b': 
        if(!grid_isWall(Grid, player->x-1, player->y-1)){
            return true;
        }
        break;
    case 'n': 
        if(!grid_isWall(Grid, player->x+1, player->y-1)){
            return true;
        }
        break;
    default:
        return false;
    }
    return false;
}

static double calculate_slope(int x1, int y1, int x2, int y2){

    if(y1 == y2){
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

int
grid_getRows(grid_t* grid)
{
  if (grid != NULL) {
    return grid->rows;
  }
  fprintf(stderr, "Grid is NULL. Stop.\n");
  return -1;
}

int
grid_getColumns(grid_t* grid)
{
  if (grid != NULL) {
    return grid->cols;
  }
  fprintf(stderr, "Grid is NULL. Stop.\n");
  return -1;
}

char**
grid_getGrid(grid_t* grid)
{
  if (grid != NULL) {
    return grid->g;
  }
  fprintf(stderr, "Attempt to get grid representation in a NULL grid. Stop.\n");
  return NULL;
}