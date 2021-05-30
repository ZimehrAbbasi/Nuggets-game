#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
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
static void game_close(gamestate_t* gameState);
void handleInput(void* arg);


int
main(const int argc, const char* argv[])
{
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

    // Free all gamestate memory
    game_close(gs);

    // Free seed value
    free(seed);
}

void 
parseArgs(int argc, char* argv[], int* seed)
{
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

static int
numberOfColumns(FILE* mapfile)
{
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

static
gamestate_t* game_init(FILE* mapFile)
{

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

static void
game_close(gamestate_t* gameState)
{

  if(gameState == NULL){
    fprintf(stderr, "Error: GameState Null...\n");
    exit(1);
  } else {
    // Close game and free memory
    gamestate_closeGame(gameState);
  }
}

// void handleInput(void* arg){

// }

char**
tokenize(char* message)
{
  if (message == NULL) {
    return NULL;
  }
  /* init position in tokens */
  int pos = 0;

  /* allocate memory for char pointers, max = strlen */
  char** tokens = calloc(strlen(message), sizeof(char*));

  if (tokens == NULL) {
    return NULL;
  }

  char* token = strtok(message, " ");

  /* while next token is not NULL 
     save it and get next token */
  while (token != NULL) {
    tokens[pos++] = malloc(sizeof(token));
    strcpy(tokens[pos-1], token);
    token = strtok(NULL, " ");
  }
  return tokens;
}

void
deleteTokens(char** parsedMessage)
{
  if (parsedMessage != NULL) {

    /* step through the tokens and free each token */
    for (int i = 0; parsedMessage[i] != NULL; i++) {
      free(parsedMessage[i]);
    }

    /* free the holder */
    free(parsedMessage);
  }
}

void
handleMessage(void* arg, const addr_t fromAddress, char* message)
{
  const gamestate_t* state = (gamestate_t*) arg;
  /* get tokens in message */
  char** tokens;
  if ( (tokens = tokenize(message)) != NULL) {

    /* find number of tokens */
    int numTokens = 0;
    for (int i = 0; tokens[i] != NULL; i++) {
      numTokens = i + 1;
    }

    if (numTokens == 0) {
      fprintf(stderr, "Message detected with ZERO tokens. Stop.\n");
      free(message);
      free(tokens);
      return;
    }

    /* run switch statement on first char of first token */
    switch (tokens[0][0])
    {
      case 'K':
        if (numTokens == 2 && (strcmp(tokens[0], "KEY") == 0) ) {

          /* call function to handle key here */
          handleKey(state, fromAddress, tokens[1]);
        }
        else {
          fprintf(stderr, "Invalid action sequence detected. Stop.\n");
        }
        break;

      case 'S':
        if (numTokens == 1 && (strcmp(tokens[0], "SPECTATE") == 0) ) {
          gamestate_addSpectator(state, fromAddress);
        }

      case 'P':
        /* routine to add player */
        if (numTokens >= 2 && (strcmp(tokens[0], "PLAY") == 0) ) {

          /* init grid for player */
          grid_t* playerGrid = grid_initForPlayer(state->masterGrid);

          /* generate letter for player */
          char letter = 'A' + state->players_seen;

          /* get full player name */
          char* playerName[100];
          for (int i = 1; i < numTokens; i++) {
            strcat(playerName, tokens[i]);
          }

          /* generate random x,y values 
             and check for validity 
             until valid point is found */
          int x = randomInt(1, state->masterGrid->cols);
          int y = randomInt(1, state->masterGrid->rows);

          while (! grid_isSpace(state->masterGrid, x, y)) {
            x = randomInt(1, state->masterGrid->cols);
            y = randomInt(1, state->masterGrid->rows);
          }

          /* create player */
          player_t* newPlayer = player_init(letter, playerName, fromAddress, x, y, playerGrid);

          /* if player created successfully,
             add to gamestate */
          if (newPlayer != NULL) {
            gamestate_addPlayer(newPlayer);
          }

          /* if player creation failed, 
             delete player grid
             free
             free tokens
             print error flag */
          else {
            grid_delete(playerGrid);
            
            fprintf(stderr, "'%s' is not a valid add player message.\n", message);
            fprintf(stderr, "Invalid action sequence detected. Stop.\n");
            
          }
        }
        else {
          fprintf(stderr, "Invalid action sequence detected. Stop.\n");
          return;
        }
        break;

    }
    deleteTokens(tokens);
    free(message);
  }

  
}

/**************** Static Functions ******************/

/**
 * @brief: Function to generate a number within a range.
 * repeatedly calls rand() until value
 * between the lower and upper bound is generated.
 * 
 * @param lower: lower bound, inclusive
 * @param upper: upper bound, inclusive
 * @return int: a random value between the lower and upper bound.
 */
static int
randomInt(int lower, int upper)
{
  if (lower < upper) {
    int num = rand();
    while (num < lower || num > upper) {
      num = rand();
    }
    return num;
  }
  fprintf(stderr, "Attempt to generate a number with invalid bounds. Stop.\n");
  return -1;
}
