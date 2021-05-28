#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
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

    grid_t* Grid  = malloc(sizeof(grid_t));
    if(Grid == NULL){
        return NULL;
    }
    char ** grid = Grid->g;
    grid = malloc(rows);
    for(int i = 0;i<rows;i++){
        grid[i] = malloc(cols);
    }

    char* line;
    int j = 0;
    while((line = file_readLine(mapfile) != NULL)){
        for(int i = 0; i < strlen(line); i++){
            grid[j][i] = line[i];
        }
        j += 1;
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

bool grid_isWall(grid_t* Grid, int x, int y){
    char **master = Grid->g;
    if(master[y][x] == "|" or master[y][x] == "-" or master[y][x] == "+" or master[y][x] == " "){
        return true;
    }
    return false;
}

bool grid_isPlayer(grid_t* Grid, int x, int y){
    char **master = Grid->g;
    if(isalpha(master[y][x])){
        return true;
    }
    return false;
}

bool grid_isGold(grid_t* Grid, int x, int y){
    char **master = Grid->g;
    if(master[y][x] == '*'){
        return true;
    }
    return false;
}

bool grid_isPassage(grid_t* Grid, int x, int y){
    char **master = Grid->g;
    if(master[y][x] == '#'){
        return true;
    }
    return false;
}

bool grid_canMove(grid_t* Grid, player_t* player, char k){
    char letter = k;
    switch (letter) {
    case 'l': 
        if(!grid_isWall(Grid, player.x+1, player.y)){
            return true
        }
        break;
    case 'h': 
        if(!grid_isWall(Grid, player.x-1, player.y)){
            return true
        }
        break;
    case 'k': 
        if(!grid_isWall(Grid, player.x, player.y+1)){
            return true
        }
        break;
    case 'j': 
        if(!grid_isWall(Grid, player.x, player.y-1)){
            return true
        }
        break;
    case 'u': 
        if(!grid_isWall(Grid, player.x+1, player.y+1)){
            return true
        }
        break;
    case 'y': 
        if(!grid_isWall(Grid, player.x-1, player.y+1)){
            return true
        }
        break;
    case 'b': 
        if(!grid_isWall(Grid, player.x-1, player.y-1)){
            return true
        }
        break;
    case 'n': 
        if(!grid_isWall(Grid, player.x+1, player.y-1)){
            return true
        }
        break;
    default:
        return false;
    }
    return false;
}

static double calculate_slope(int x1, int y1, int x2, int y2){

    if(y1 == y2){
        return NULL;
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
    char **player_grid = player-grid->g;
    double slope;
    double x_pred, y_pred;
    int upper, lower;
    bool visibility;
    for(int x = 0; x < Grid.cols; x++){
        for(int y = 0; y < Grid.rows; y++){
            slope = calculate_slope(x, y, player.x, player.y);
            visibility = true;
            if(abs(player.y - y) > abs(player.x - x)){
                for(int y1 = min(y, player.y)+1; y1 < max(y, player.y); y1++){
                    x_pred = func(y1, min(x, player.x), -1/slope);
                    upper = (int)ceil(x_pred);
                    lower = (int)floor(x_pred);
                    
                    if(master_grid[y1][upper] != '.' || master_grid[y1][lower] != '.' || !isalpha(master_grid[y1][upper]) || !isalpha(master_grid[y1][lower])){
                        visibility = false;
                        break;      
                    }
                }
            }else{
                for(int x1 = min(x, player.x); x1 < max(x, player.x); x1++){
                    y_pred = func(x1, min(y, player.y), slope);
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
            }

        }
    }
}