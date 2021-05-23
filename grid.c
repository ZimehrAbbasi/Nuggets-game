#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "support/file.h"
#include "support/message.h"
#include "support/log.h"

typedef struct grid {
    char **g;
    int rows;
    int cols;
} grid_t;

grid_t* grid_init(File* mapfile, int rows, int cols){
    if(mapfile == NULL || rows == 0 || cols == 0){
        return NULL;
    }

    grid_t* grid  = malloc(sizeof(grid_t));
    if(grid == NULL){
        return NULL;
    }

    grid->g = malloc(rows);
    for(int i = 0;i<rows;i++){
        grid->g[i] = malloc(cols);
    }

    return grid;
}

char* grid_toString(grid_t* Grid){
    if(Grid == NULL){
        return NULL;
    }
    char* stringifiedGrid = malloc(Grid.rows * Grid.cols);
    char ** grid = Grid->g;
    for(int x = 0; x < Grid.rows; x++){
        for(int y = 0; y < Grid.cols; y++){
            sprintf(stringifiedGrid, "%c", grid[y]);
        }
        sprintf(stringifiedGrid, "\n");
    }
    return stringifiedGrid;
}

bool grid_isWall(grid_t* grid, int x, int y){
    char **master = grid->g;
    if(master[y][x] == "|" or master[y][x] == "-" or master[y][x] == "+" or master[y][x] == " "){
        return true;
    }
    return false;
}

bool grid_isPlayer(grid_t* grid, int x, int y){
    char **master = grid->g;
    if(isalpha(master[y][x])){
        return true;
    }
    return false;
}

bool grid_isGold(grid_t* grid, int x, int y){
    char **master = grid->g;
    if(master[y][x] == '*'){
        return true;
    }
    return false;
}

bool grid_isPassage(grid_t* grid, int x, int y){
    char **master = grid->g;
    if(master[y][x] == '#'){
        return true;
    }
    return false;
}

bool grid_canMove(grid_t* master, player_t* player, char k){
    
}