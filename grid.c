#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


#include "file.h"         /* file operations */
#include "message.h"      /* message operations */
#include "player.h"       /* player module */
#include "gamestate.h"    /* gamestate module */
#include "grid.h"         /* self */

typedef struct grid {
  char **g;
  int rows;
  int cols;
} grid_t;

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
    grid->g = malloc(rows);

    /* allocate rows */
    for(int i=0; i<rows; i++){
      grid->g[i] = malloc(cols);
    }

    /* copy lines from file as rows */
    int j = 0;
    char* line;
    while( (line = file_readLine(mapfile)) != NULL){
      strcpy(grid->g[j++], line);
    }

    /* return grid */
    return grid;
  }

  /* in case pointer to map file is NULL */
  fprintf(stderr, "Attempt to create grid with a NULL name of map file. Stop.\n");
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
}

bool
grid_isWall(grid_t* grid, int x, int y)
{
  char **master = grid->g;
  return ( (master[y][x] == "|") || 
           (master[y][x] == "-") || 
           (master[y][x] == "+") || 
           (master[y][x] == " ") );
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
  char **master = grid->g;
  return master[y][x] == '#';
    // {
    //     return true;
    // }
    // return false;
}

bool 
grid_canMove(grid_t* master, player_t* player, char k)
{
  // if (master != NULL && player != NULL) {
  //     switch (k) {
  //         case 
  //     }
  // }
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