#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "support/file.h"
#include "support/message.h"
#include "support/log.h"
#include "gamestate.h"
#include "grid.h"
#include "gold.h"

int MaxNameLength = 50;
int MaxPlayers = 26;
int GoldTotal = 250;
int GoldMinNumPiles = 10;
int GoldMaxNumPiles = 30;


// Function prototypes
void parseArgs(int argc, char* argv[], int* seed);
static int numberOfColumns(FILE* mapfile);
static void gold_distribute(grid_t* Grid, gold_t* Gold);
static gamestate_t* game_init(FILE* mapfile);
//static void game_close(gamestate_t* gameState, (close_func)(*void arg));
void handleInput(void* arg);


int main(const int argc, const char* argv[]){
    // Parse arguments  and use seed value
    int* seed = malloc(sizeof(int));
    parseArgs(argc, argv, &seed);
    srand(*seed);

    // Open and close map file and init gamestate object
    FILE* fp  = fopen(argv[1], "r");
    gamestate_t* gs = game_init(fp);
    fclose(fp);

    // Initialize network and get port number
    int port = message_init(stderr);
    if(port == 0){
        fprintf(stderr, "Could not initialize message...\n");
        exit(1);
    }

    // // Start message loop
    // message_loop(
    //     /**/
    //     ,/**/
    //     ,/**/
    //     ,/**/
    // );

    //game_close(gs, NULL);

    // Free seed value
    free(seed);
}

void parseArgs(int argc, char* argv[], int* seed){
    // Check for illegal # of arguments
    if(argc != 3){
        fprintf(stderr, "Illegal number of arguments...\n");
        exit(1);
    }

    // Make sure seed is a valid number
    for(int i = 0; i < strlen(argv[2]); i++){
        if(!isdigit(argv[2][i])){
            fprintf(stderr, "Invalid seed...\n");
            exit(1);
        }
    }

    // Set seed value
    *seed = atoi(argv[2]);

    // Try to open map file
    FILE* fp;
    if((fp = fopen(argv[1], "r") == NULL)){
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


// static void gold_distribute(grid_t* Grid, gold_t* Gold){
//     char **grid = Grid->g;

//     i = 0;
//     int x, y;

//     while(i < Gold->numPiles){
//         x = rand(1, Grid->cols);
//         y = rand(1, Grid->rows);
//         if(grid[y][x] == '.'){
//             grid[x][y] = '*';
//             i += 1;
//         }
//     }
// }

static gamestate_t* game_init(FILE* mapFile){

    if(mapFile == NULL){
        fprintf(stderr, "Unable to open and read map file");
        exit(1);
    }
    
    // Create gamestate pointer and call gamestate_init()
    gamestate_t* gameState = gamestate_init(mapFile);

    // Condition: gamestate_init successfully created an object
    if(gameState == NULL){
        // If gamestate_init gives a NULL poiter, exit with error
        fprintf(stderr, "Unable to allocate space for the game state.\n");
        exit(1);
    }else{
        // Distribute gold throughout the grid
        gold_distribute(gameState->masterGrid, gameState->gameGold);
    }

    // Return the gamestate object
    return gameState;
}

// static void game_close(gamestate_t* gameState, *(close_func)(*void arg)){

//     if(gameState == NULL){
//         fprintf(stderr, "Error: GameState Null...\n");
//         exit(1);
//     }

//     if(close_func == NULL){
//         free(gameState->gameGold->totalGold);
//         free(gameState->gameGold);
//         for(int i = 0; i < 26; i++){
//             if(gameState->allPlayers == NULL){
//                 break;
//             }
//             free(gameState->allPlayers[i]->address);
//             free(gameState->allPlayers[i]);
//         }
//         free(gameState->allPlayers);
//         grid_t* grid = gameState->master_grid;
//         for(int i = 0;i<rows;i++){
//             free(grid->g[i]);
//         }
//         free(grid->g);
//         free(gameState);
//     }
// }

// void handleInput(void* arg){

// }