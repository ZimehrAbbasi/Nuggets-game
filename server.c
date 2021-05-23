#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "support/file.h"
#include "support/message.h"
#include "support/log.h"
#include "grid.h"

int MaxNameLength = 50;
int MaxPlayers = 26;
int GoldTotal = 250;
int GoldMinNumPiles = 10;
int GoldMaxNumPiles = 30;

typedef struct gameState{
    struct player *allPlayers;
    struct gold gameGold;
    struct grid master_grid; 
    int numRows;
    int numCols;
} gameState_t;

typedef struct player{
    addr_t* address;
    int gold;
    int x;
    int y;
    char *player_grid[][];
} player_t;

typedef struct gold{
    int* totalGold;
    int numPiles;
    int index;
} gold_t;

void parseArgs(int argc, char* argv[], int* seed){
    if(argc != 3){
        fprintf(stderr, "Illegal number of arguments...\n");
        exit(1);
    }


    for(int i = 0; i < strlen(argv[2]); i++){
        if(!isdigit(argv[2][i])){
            fprintf(stderr, "Invalid seed...\n");
            exit(1);
        }
    }

    *seed = atoi(argv[2]);

    FILE* fp;
    if((fp = fopen(argv[1]) == NULL){
        fprintf(stderr, "Could not open file...\n");
        exit(1);
    }
    fclose(fp);
}

static int numberOfColumns(FILE* mapfile){
    char* line = file_readLine(mapfile);

    if(line == NULL) return 0;

    int len = strlen(line);
    free(line);

    return len;
}

static void gold_init(gold_t* gold, int numPiles){
    int remainingGold = GoldTotal;
    for(int i = 0; i < numPiles; i++){
        goldTotal[i] = rand(1, remainingGold - (numPiles - i - 1))
    }
}

static void gold_distribute(grid_t* Grid, gold_t* Gold){
    char **grid = Grid->g;

    i = 0;
    int x, y;

    while(i < Gold->numPiles){
        x = rand(1, Grid->cols);
        y = rand(1, Grid->rows);
        if(grid[y][x] == '.'){
            grid[x][y] = '*';
            i += 1;
        }
    }
}

static gameState_t* game_init(FILE* mapfile){

    if(mapfile == NULL){
        return NULL;
    }
    
    gameState_t* gameState = malloc(sizeof(gameState_t));

    if(gameState == NULL){
        return gameState;
    }else{
        gameState->allPlayers = malloc(sizeof(player_t)*26);
        gameState->gameGold = malloc(sizeof(gold_t));
        int numPiles = rand(10, 30);
        gameState->gameGold->totalGold = malloc(numPiles*sizeof(int));
        gameState->gameGold->numPiles = numPiles;
        gameState->gameGold->index = 0;
        int numRows = file_numLines(mapfile);
        int numCols = numberOfColumns(mapfile);
        gameState->master_grid = grid_init(mapfile, numRows, numCols);
        if(gameState->master_grid == NULL){
            fprintf(stderr, "Could not create grid...\n");
            exit(1);
        }
        gameState->master_grid.rows = numRow;
        gameState->master_grid.cols = numCols;
        gold_init(gameState->gameGold, numPiles);
        gold_distribute(gameState->master_grid, gameState->gameGold);
    }
    return gameState;
}

static void game_close(gameState_t* gameState, *(close_func)(*void arg)){

    if(gameState == NULL){
        fprintf(stderr, "Error: GameState Null...\n");
        exit(1);
    }

    if(close_func == NULL){
        free(gameState->gameGold->totalGold);
        free(gameState->gameGold);
        for(int i = 0; i < 26; i++){
            if(gameState->allPlayers == NULL){
                break;
            }
            free(gameState->allPlayers[i]->address);
            free(gameState->allPlayers[i]);
        }
        free(gameState->allPlayers);
        grid_t* grid = gameState->master_grid;
        for(int i = 0;i<rows;i++){
            free(grid->g[i]);
        }
        free(grid->g);
        free(gameState);
    }
}

void handleInput(void* arg){

}

int main(const in argc, const char* argv[]){

    int* seed = 0;
    parseArgs(argc, argv, &seed);
    srand(*seed);
    FILE* fp  = fopen(argv[1], "r");
    gameState_t* gs = game_init(fp);
    fclose(fp);

    int port = message_init(stderr);
    if(port == 0){
        fprintf(stderr, "Could not initialize message...\n");
        exit(1);
    }
    message_loop();
    game_close(gs)
}