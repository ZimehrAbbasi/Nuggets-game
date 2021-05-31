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

char* grid_toString(grid_t* grid){
  if (grid == NULL){
    return NULL;
  }
  char* stringifiedGrid = malloc(grid->rows * grid->cols);
  char** map = grid->g;
  for(int x = 0; x < grid->rows; x++){
    strcat(stringifiedGrid, map[x]);
    if (x != grid->rows-1) {
      strcat(stringifiedGrid, "\n");
    }  
      // for(int y = 0; y < Grid->cols; y++){
      //     sprintf(stringifiedGrid, "%c", grid[y]);
      // }
      // sprintf(stringifiedGrid, "\n");
  }
  return stringifiedGrid;
}

char* 
grid_masterGridToString(grid_t* grid, gamestate_t* gamestate)
{
  char* map = grid_toString(grid);
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

    return (x1 - x2)/(y1 - y2);
}

static double func(int x, int y, double slope){
    return slope * x + y;
}

static int max(int num1, int num2){
    return num1 > num2 ? num1 : num2;
}

static int min(int num1, int num2){
    return num1 > num2 ? num2 : num1;
}

void grid_calculateVisibility(grid_t* Grid, player_t* player){

    char **master_grid = Grid->g;
    char **player_grid = player->grid->g;
    double slope;
    double x_pred, y_pred;
    int upper, lower;
    bool visibility;
    for(int x = 0; x < Grid->cols; x++){
        for(int y = 0; y < Grid->rows; y++){
            slope = calculate_slope(x, y, player->x, player->y);
            visibility = true;
            if(abs(player->y - y) > abs(player->x - x)){
                for(int y1 = min(y, player->y)+1; y1 < max(y, player->y); y1++){
                    x_pred = func(y1, min(x, player->x), -1/slope);
                    upper = (int)ceil(x_pred);
                    lower = (int)floor(x_pred);
                    
                    if(master_grid[y1][upper] != '.' || master_grid[y1][lower] != '.' || !isalpha(master_grid[y1][upper]) || !isalpha(master_grid[y1][lower])){
                        visibility = false;
                        break;      
                    }
                }
            }else{
                for(int x1 = min(x, player->x); x1 < max(x, player->x); x1++){
                    y_pred = func(x1, min(y, player->y), slope);
                    upper = (int)ceil(y_pred);
                    lower = (int)floor(y_pred);
                    
                    if(master_grid[upper][x1] != '.' || master_grid[lower][x1] != '.' || !isalpha(master_grid[upper][x1]) || !isalpha(master_grid[lower][x1])){
                        visibility = false;
                        break;      
                    }
                }
            }

            if(visibility){
                player_grid[y][x] = master_grid[x][y];
            }else{
                if(grid_isGold(Grid, x, y)){
                    player_grid[y][x] = '.';
                }
            }

        }
    }

}

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