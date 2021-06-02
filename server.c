/**
 * @file server.c
 * @author TEAM PINE
 * @brief: defines functionality for the server,
 * which interacts with other modules for functionality
 * to offer an interactive game.
 * @version 0.1
 * @date 2021-06-01
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

#include "file.h"         /* file operations */
#include "message.h"      /* message operations */
#include "log.h"          /* logging operations */
#include "gamestate.h"    /* gamestate module */
#include "grid.h"         /* grid module */
#include "gold.h"         /* gold module */
#include "player.h"       /* player module */
#include "spectator.h"    /* spectator module */

// Global Variables
const int MaxNameLength = 50;
const int MaxPlayers = 26;
int GoldTotal = 250;
const int GoldMinNumPiles = 10;
const int GoldMaxNumPiles = 30;


// Function prototypes
void parseArgs(const int argc, const char* argv[], int* seed);
static gamestate_t* game_init(FILE* mapfile);
static void game_close(gamestate_t* gameState);
void handleInput(void* arg);
char** tokenize(char* message);
void deleteTokens(char** parsedMessage);
bool handleMessage(void* arg, const addr_t fromAddress, const char* message);
void movePlayer(gamestate_t* gameState, player_t* player, int x, int y);
static void handlePlayerQuit(gamestate_t* state, addr_t fromAddress);
static void addSpectatorToGame(gamestate_t* state, addr_t fromAddress);
static void reportMalformedMessage(addr_t fromAddress, char* givenInput, char* message);
static int randomInt(int lower, int upper);
static void handleSpectatorQuit(gamestate_t* state, addr_t fromAddress);
static bool isGameEnded(gamestate_t* state);
static void displayForSpectator(gamestate_t* state, spectator_t* spectator);
static void displayForPlayer(gamestate_t* state, player_t* player);
static int getRemainingGold(gamestate_t* state);
static void sendGoldToPlayers(gamestate_t* state);
static void sendPlayerOK(player_t* player);
static void sendGoldToSpectator(gamestate_t* state);
static void handleKey(gamestate_t* state, addr_t fromAddress, char pressedKey);
static void playerPickedUpGold(gamestate_t* state, player_t* player, int justCollectedGold);
static void endGame(gamestate_t* state);

void 
parseArgs(const int argc, const char* argv[], int* seed)
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
  if((fp = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Could not open file...\n");
    exit(1);
  }
  fclose(fp);
}

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


char**
tokenize(char* message)
{
  if (message == NULL) {
    return NULL;
  }
  /* init position in tokens */
  int pos = 0;

  /* allocate memory for char pointers, max = strlen */
  char** tokens = calloc(strlen(message), sizeof(*tokens));

  if (tokens == NULL) {
    return NULL;
  }

  char* token = strtok(message, " ");

  /* while next token is not NULL 
     save it and get next token */
  while (token != NULL) {
    tokens[pos++] = malloc(sizeof(token)+1);
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

bool
handleMessage(void* arg, const addr_t fromAddress, const char* message)
{
  /* if any pointer arguments are NULL, return true and terminate game loop. */
  if (arg == NULL || message == NULL) {
    fprintf(stderr, "Entered message loop without gamestate. Fatal error occurred");
    return true;
  }

  /* convert arg back to gamestate */
  gamestate_t* state = (gamestate_t*) arg;
  /* get tokens in message */
  char* message_copy = calloc(strlen(message)+1, sizeof(*message_copy));
  strcpy(message_copy, message);
  char** tokens;
  if ( (tokens = tokenize(message_copy)) != NULL) {

    /* find number of tokens */
    int numTokens = 0;
    for (int i = 0; tokens[i] != NULL; i++) {
      numTokens = i + 1;
    }

    if (numTokens == 0) {
      // Send malformed message back to client / spectator
      message_send(fromAddress, "ERROR malformed message\n");
      fprintf(stderr, "Message detected with ZERO tokens. Stop.\n");
      free((char*)message);
      free(message_copy);
      free(tokens);
      return false;
    }

    /* run switch statement on first char of first token */
    switch (tokens[0][0])
    {
      case 'K':

        /* handle gameplay keys */
        if (numTokens == 2 && (strcmp(tokens[0], "KEY") == 0) ) {
          /* call function to handle key here */
          handleKey(state, fromAddress, tokens[1][0]);
        }
        else {
          reportMalformedMessage(fromAddress, message_copy, "is not a valid gameplay message.");
        }
        break;

      case 'S':

        /* add spectator if message checks out*/
        if (numTokens == 1 && (strcmp(tokens[0], "SPECTATE") == 0) ) {

          /* add spectator to game */
          addSpectatorToGame(state, fromAddress);
        }
        else {
          // Oherwise end error message to from address and log to stderr
          reportMalformedMessage(fromAddress, message_copy, "is not a valid add spectator message.");
        }
        break;
      case 'Q':
        /* routine for removing player */
        if (numTokens == 0 && (strcmp(tokens[0], "QUIT") == 0) ) {

          /* if address matches spectator in the game... */
          if (gamestate_isSpectator(state, fromAddress)) {
            // Handle spectator quit
            handleSpectatorQuit(state, fromAddress);
          }

          /* else -->
             if address doesn't match the current spectator,
             search for player instead.
             
             then, if, no match was found. print error message. */
          else {
            handlePlayerQuit(state, fromAddress);
          }
        }
        break;

      case 'P':
        /* routine to add player */
        if (numTokens >= 2 && (strcmp(tokens[0], "PLAY") == 0) ) {

          /* init grid for player */
          grid_t* playerGrid = grid_initForPlayer(state->masterGrid);

          /* get number of rows and columns in master grid */
          int rows = state->masterGrid->rows;
          int cols = state->masterGrid->cols;

          /* generate letter for player */
          char letter = 'A' + state->players_seen;

          /* get full player name */
          char playerName[MaxNameLength];
          for (int i = 1; i < numTokens; i++) {
            char *temp = tokens[i];
            if(i == 1){
              strcpy(playerName, temp);
            }else{
              strcat(playerName, temp);
            }
          }

          /* generate random x,y values 
             and check for validity 
             until valid point is found */
          int x = randomInt(1, cols);
          int y = randomInt(1, rows);

          while (! grid_isSpace(state->masterGrid, x, y)) {
            x = randomInt(1, cols);
            y = randomInt(1, rows);
          }

          /* create player */
          player_t* newPlayer = player_new(letter, playerName, fromAddress, x, y, playerGrid);

          /* if player created successfully,
             add to gamestate */
          if (newPlayer != NULL) {
            gamestate_addPlayer(state, newPlayer);
            char initMessage[100];
            sprintf(initMessage, "GRID %d %d", rows, cols);
            player_send(newPlayer, initMessage);
            sendPlayerOK(newPlayer);
          }

          /* if player creation failed, 
             delete player grid
             print error flag */
          else {
            grid_delete(playerGrid);
            reportMalformedMessage(fromAddress, message_copy, "is not a valid player message.");
          }
        }
        else {
          reportMalformedMessage(fromAddress, message_copy, "is not a valid message.");
        }
        break;

    }
    deleteTokens(tokens);
    //free((char*)message);
    free(message_copy);
  }

  // Send updated game state to spectator
  displayForSpectator(state, state->spectator);

  // Send updated game state to all players
  int numClients = state->players_seen;
  player_t** clients = state->players;
  for(int i = 0; i < numClients; i++){
    displayForPlayer(state, clients[i]);
  }

  // Send gold to players
  sendGoldToPlayers(state);

  // Send gold to spectator
  sendGoldToSpectator(state);

  // Check if game is ended
  if(!isGameEnded(state)){
    return false;
  }else{
    // Do things for when game is over
	endGame(state);
    return true;
  }
}

/**************** Static Functions ******************/

static void
playerPickedUpGold(gamestate_t* state, player_t* player, int justCollectedGold){
  int currentPlayerGold = player->gold;
  int goldLeftInGame = getRemainingGold(state);

  // Format individual GOLD message and send to player
  char goldCollectedMessage[50];
  sprintf(goldCollectedMessage, "GOLD %d %d %d", justCollectedGold, currentPlayerGold, goldLeftInGame);
  player_send(player, goldCollectedMessage);

  // Send GOLD messages to all players and spectators
  sendGoldToPlayers(state);
  sendGoldToSpectator(state);
}

/**
 * @brief: Function to generate a number within a range.
 * calls rand() to generate a random number 
 * then bounds it to expectate range using mod operator.
 * This is handy because rand() doesn't take any arguments
 * and generates unbounded values
 * 
 * @param lower: lower bound, inclusive
 * @param upper: upper bound, exclusive
 * @return int: a random value between the lower and upper bound.
 */
static int
randomInt(int lower, int upper)
{
  if (lower < upper) {
    int num = lower;
    int randomNumber = rand();
    num += randomNumber % (upper - lower);
    return num;
  }
  fprintf(stderr, "Attempt to generate a number with invalid bounds. Stop.\n");
  return -1;
}

static void
addSpectatorToGame(gamestate_t* state, addr_t fromAddress){
  /* add spectator to game */
  gamestate_addSpectator(state, fromAddress);

  /* get number of rows, columns in grid */
  int rows = state->masterGrid->rows;
  int cols = state->masterGrid->cols;

  /* generate init message */
  char initMessage[100];                            /* No need to malloc; only used once */
  sprintf(initMessage, "GRID %d %d", rows, cols);

  /* send init message to spectator */
  spectator_send(state->spectator, initMessage);
}

static void
reportMalformedMessage(addr_t fromAddress, char* givenInput, char* message){
  message_send(fromAddress, "ERROR malformed message\n");
  fprintf(stderr, "'%s' %s \n", givenInput, message);
  fprintf(stderr, "Invalid action sequence detected. Stop.\n");
}

static void
handleSpectatorQuit(gamestate_t* state, addr_t fromAddress){
  /* get spectator */
  spectator_t* spectator = state->spectator;

  /* send QUIT message to spectator */
  spectator_send(spectator, "QUIT Thank you for watching!");

  /* delete spectator */
  spectator_delete(spectator);

  /* reset state spectator pointer to NULL 
     (just so we don't get any surprises) */
  state->spectator = NULL;
}

static void
handleKey(gamestate_t* state, addr_t fromAddress, char pressedKey){
	// Get player object from address
	player_t* player = gamestate_findPlayerByAddress(state, fromAddress);
	grid_t* Grid = state->masterGrid;

	// If cant find player but can find spectator
	if (player == NULL &&  message_eqAddr(fromAddress, state->spectator->address) ){
		fprintf(stderr, "Couldn't find a matching player for key press");
    if (pressedKey == 'Q'){
      handleSpectatorQuit(state, fromAddress);
    }
		return;
	}

    switch (pressedKey) {
    case 'l': 
        movePlayer(state, player, player->x+1, player->y);
        break;
    case 'h': 
        movePlayer(state, player, player->x-1, player->y);
        break;
    case 'k': 
        movePlayer(state, player, player->x, player->y-1);
        break;
    case 'j': 
        movePlayer(state, player, player->x, player->y+1);
        break;
    case 'u': 
        movePlayer(state, player, player->x+1, player->y-1);
        break;
    case 'y': 
        movePlayer(state, player, player->x-1, player->y-1);
        break;
    case 'b': 
        movePlayer(state, player, player->x-1, player->y+1);
        break;
    case 'n': 
        movePlayer(state, player, player->x+1, player->y+1);
        break;
    case 'L': 
        while(!grid_isWall(Grid, player->x+1, player->y)){
            movePlayer(state, player, player->x+1, player->y);
        }
        break;
    case 'H': 
        while(!grid_isWall(Grid, player->x-1, player->y)){
            movePlayer(state, player, player->x-1, player->y);
        }
        break;
    case 'K': 
        while(!grid_isWall(Grid, player->x, player->y-1)){
            movePlayer(state, player, player->x, player->y-1);
        }
        break;
    case 'J': 
        while(!grid_isWall(Grid, player->x, player->y+1)){
            movePlayer(state, player, player->x, player->y+1);
		}
        break;
    case 'U': 
        while(!grid_isWall(Grid, player->x+1, player->y-1)){
            movePlayer(state, player, player->x+1, player->y-1);
        }
        break;
    case 'Y': 
        while(!grid_isWall(Grid, player->x-1, player->y-1)){
            movePlayer(state, player, player->x-1, player->y-1);
        }
        break;
    case 'B': 
        while(!grid_isWall(Grid, player->x-1, player->y+1)){
            movePlayer(state, player, player->x-1, player->y+1);
        }
        break;
    case 'N': 
        while(!grid_isWall(Grid, player->x+1, player->y+1)){
            movePlayer(state, player, player->x+1, player->y+1);
        }
        break;
    case 'Q':
      handlePlayerQuit(state, fromAddress);
      break;
    default:
        break;
    }
}

static void
handlePlayerQuit(gamestate_t* state, addr_t fromAddress){
  /* We don't actually delete the player from the game 
    because we need to keep their information
    until end of game. */

  // Search for player with matching address in gamestate
  player_t* player = gamestate_findPlayerByAddress(state, fromAddress);

  // If we find a matching player in the game, let them quit
  if (player != NULL) {
    player->hasQuit = true;
    player_send(player, "QUIT Thank you for playing!");
  }

  /* if the search function returns NULL, 
     no player was found. 
     print to stderr. */
  else {
    fprintf(stderr, "No matching player OR spectator found for an incoming QUIT message.\n");
  }
}

static void
displayForSpectator(gamestate_t* state, spectator_t* spectator){
  // Convert master grid to a string
  grid_t* entireGrid = state->masterGrid;
  char* masterGridAsString = grid_toString(state, entireGrid);
  
  // Create message header
  char* messageHeader = malloc((sizeof(char) * strlen(masterGridAsString)) + 10 );
  
  // Concatenate and send message
  strcpy(messageHeader, "DISPLAY\n");
  strcat(messageHeader, masterGridAsString);
  spectator_send(spectator, messageHeader);

  // Free created memory
  free(messageHeader);
  free(masterGridAsString);
}

static void
displayForPlayer(gamestate_t* state, player_t* player){
  // Update player's visible grid
  grid_t* entireGrid = state->masterGrid;
  grid_calculateVisibility(entireGrid, player);

  // Covert visible grid to string
  char* playerGridAsString = grid_toStringForPlayer(state, player);
  
  // Create message header
  char* messageHeader = malloc((sizeof(char) * strlen(playerGridAsString)) + 10 );
  strcpy(messageHeader, "DISPLAY\n");

  /* TODO: Make a playerSpecific grid_toString function that takes in
  * the game state so that player and gold visibility can be accounted for
  * in the final string.
  */ 
  strcat(messageHeader, playerGridAsString);
  player_send(player, messageHeader);

  // Free created memory
  free(messageHeader);
  free(playerGridAsString);
}

static bool
isGameEnded(gamestate_t* state){
  // Get gold object from game state
  gold_t* gold_from_state = state->gameGold;
  
  // See if we have collected all gold piles
  // (if index == num_gold_piles)
  int index = gold_from_state->index;
  int num_gold_piles = gold_from_state->numPiles;


  if(index >= num_gold_piles){
    return true;
  } else{
    return false;
  }
}

void movePlayer(gamestate_t* gameState, player_t* player, int x, int y){

	if(gameState == NULL){
		return;
	}

  if(x < 0 || y < 0|| x >= gameState->masterGrid->cols || y >= gameState->masterGrid->rows){
    return;
  }

	grid_t* Grid = gameState->masterGrid;
 	gold_t* gameGold = gameState->gameGold;
	
  	char** player_grid = player->grid->g;
	char** master_grid = Grid->g;
	int* gold_array = gameGold->goldCounter;

	player_t **players = gameState->players;
	player_t* otherPlayer = NULL;
	for(int i = 0; i < gameState->players_seen;i++){
		otherPlayer = players[i];
		if (otherPlayer->x == x && otherPlayer->y == y && otherPlayer->letter != player->letter){
			break;
		} else{
			otherPlayer = NULL;
		}
	}


	if(!grid_isWall(Grid, x, y)){
		if (grid_isGold(Grid, x, y)){
			player_grid[player->y][player->x] = '.';
			master_grid[player->y][player->x] = '.';

			player->x = x;
			player->y = y;
			
			player->gold += gold_array[gameGold->index];
			int goldJustCollected = gold_array[gameGold->index];
			playerPickedUpGold(gameState, player, goldJustCollected);

			gameGold->index += 1;
				
			master_grid[player->y][player->x] = '.';

		}else if(otherPlayer != NULL){

			char** otherPlayer_grid = otherPlayer->grid->g;

			int tempx = player->x;
			int tempy = player->y;

			player_grid[player->y][player->x] = '.';
			master_grid[player->y][player->x] = '.';

			otherPlayer_grid[otherPlayer->y][otherPlayer->x] = '.';
			otherPlayer_grid[otherPlayer->y][otherPlayer->x] = '.';

			player->y = otherPlayer->y;
			player->x = otherPlayer->x;

			otherPlayer->y = tempy;
			otherPlayer->x = tempx;
			
		}else{
      player_grid[y][x] = master_grid[y][x];
			player->x = x;
			player->y = y;
		}
	}
}

static void endGame(gamestate_t* state){
  // Get array of players and length of players array
  player_t** allPlayers = state->players;
  int numPlayers = state->players_seen;

  // Allocate space for message to players
  char* endMessage = calloc(1+numPlayers, (MaxNameLength*sizeof(char)) + 20);

  // Loop over every player and add info to leaderboard
  strcpy(endMessage, "QUIT GAME OVER:\n");
  for(int i = 0; i < numPlayers; i++){
    char* tempBuffer = calloc(1, (MaxNameLength*sizeof(char)) + 20);

    // Concatenate new player info onto leaderboard
    sprintf(tempBuffer, "%c   %d    %s\n", allPlayers[i]->letter, allPlayers[i]->gold, allPlayers[i]->name);
    strcat(endMessage, tempBuffer);
    
    // Free associated memory
    free(tempBuffer);
  }

  // Send message to all players and the specatator and then free them
  for(int i = 0; i < numPlayers; i++){
    player_send(allPlayers[i], endMessage);
  }

  spectator_send(state->spectator, endMessage);

  // Free ending message
  free(endMessage);
}

static void
sendGoldToPlayers(gamestate_t* state){
  // Loop over every player
  player_t** allPlayers = state->players;
  int numPlayers = state->players_seen;

  for(int i = 0; i < numPlayers; i++){
    // Get n, p and r
    int currentPlayerGold = allPlayers[i]->gold;
    int justCollectedGold = 0;
    int goldLeftInGame = getRemainingGold(state);

    // Format and send message
    char* goldMessage = malloc(sizeof(char) * 100);
    sprintf(goldMessage,
    "GOLD %d %d %d", justCollectedGold, currentPlayerGold, goldLeftInGame);

    player_send(allPlayers[i], goldMessage);

    // Free used memory
    free(goldMessage);
  }
}

static void
sendGoldToSpectator(gamestate_t* state){
  // Get spectator object
  spectator_t* spectator = state->spectator;

  // Get numbers for amnt of gold
  int currentGold = 0;
  int justCollectedGold = 0;
  int goldLeftInGame = getRemainingGold(state);

  // Create gold message
  char* goldMessage = malloc(sizeof(char) * 100);
  sprintf(goldMessage, "GOLD %d %d %d", currentGold, justCollectedGold, goldLeftInGame);
  
  // Send gold message
  spectator_send(spectator, goldMessage);

  //Free created memory
  free(goldMessage);
}

static int
getRemainingGold(gamestate_t* state){
  gold_t* gamestateGold = state->gameGold;
  int goldLeft = 0;
  int currentIndex = gamestateGold->index;

  for(int i = currentIndex; i < gamestateGold->numPiles; i++){
    goldLeft += gamestateGold->goldCounter[i];
  }

  return goldLeft;
}


static void 
sendPlayerOK(player_t* player){
  // Generate message from player data
  char okMessage[10];
  sprintf(okMessage, "OK %c", player->letter);

  // Send message to player
  player_send(player, okMessage);
}

int
main(const int argc, const char* argv[])
{
  // Parse arguments  and use seed value
  int seed = 0;
  parseArgs(argc, argv, &seed);
  srand(seed);

  // Open and close map file and init gamestate object
  FILE* fp  = fopen(argv[1], "r");
  gamestate_t* gs = game_init(fp);
  if(gs == NULL){
    printf("NULL GAMESTATE!\n");
    exit(1);
  }
  fclose(fp);

  // Initialize network and get port number
  int port = message_init(stderr);
  if(port == 0){
      fprintf(stderr, "Could not initialize message...\n");
      exit(1);
  }


  // Start message loop
  message_loop(
    gs, /* Argument passed to all callbacks */
    0.0,/* Timeout specifier (0 in our case) */
    NULL,/* Handle Timeout function pointer (NULL in our case) */
    NULL, /* Handle stdin (NULL in our case) */
    handleMessage
  );

  // Free all gamestate memory
  game_close(gs);
}