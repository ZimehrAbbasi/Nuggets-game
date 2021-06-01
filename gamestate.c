/**
 * @file gamestate.c
 * @author TEAM PINE
 * @brief: functionality for gamestate
 * The gamestate holds all the other variables in the game
 * such as the gold, spectators, and player, and is passed
 * into handleMessage as a way for handleMessage to interact 
 * with the state of the ongoing game.
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "player.h"     /* player module */
#include "grid.h"       /* grid module */
#include "gold.h"       /* gold module */
#include "spectator.h"  /* spectator module */
#include "gamestate.h"  /* self */

/********* static function prototypes **********/
static void gamestate_initPlayers(gamestate_t* state);
static void gamestate_initGold(gamestate_t* state);
static void gamestate_initGrid(gamestate_t* state, FILE* mapFile);
static void gamestate_initSpectator(gamestate_t* state);
static void gamestate_playersDeleteHelper(gamestate_t* state);


/**
 * @brief constructor.
 * 
 * Inputs:
 * @param mapFile: a FILE pointer to the opened file containing map data.
 * 
 * Returns:
 * @return gamestate_t*: the initialized game instance.
 * @return NULL: an error occured allocating memory for the gamestate.
 */
gamestate_t*
gamestate_init(FILE* mapFile)
{
  gamestate_t* state = malloc(sizeof(gamestate_t));
  if (state == NULL) {
    fprintf(stderr, "Error allocating memory for gamestate.\n");
    return NULL;
  }

  // Initialize grid field
  gamestate_initGrid(state, mapFile);

  // Initialize gold field
  gamestate_initGold(state);

  // Initialize player array field
  gamestate_initPlayers(state);

  // Initialize spectator field
  gamestate_initSpectator(state);

  return state;
}

/**
 * @brief: initializes array to hold players in the game. 
 * This function allocates memory which must be free'd later
 * by calling gamestate_closeGame().
 */
static void 
gamestate_initPlayers(gamestate_t* state){
  *(state->players) = malloc(sizeof(player_t) * 26);
}

/**
 * @brief: initializes a counter for the gold 
 * in the current game session. 
 * See gamestate.h for detailed documentation.
 */
static void 
gamestate_initGold(gamestate_t* state){
  state->gameGold = gold_init(26);
}

/**
 * @brief: initializes the master grid 
 * for the current session of the game. .
 */
static void
gamestate_initGrid(gamestate_t* state, FILE* mapFile){
  state->masterGrid = grid_init(mapFile);
}

/**
 * @brief: initializes the spectator 
 * in the current session of the game.
 * The spectator is set to NULL until one joins the game.
 */
static void
gamestate_initSpectator(gamestate_t* state){
  state->spectator = NULL;
}

/**
 * @brief a static function that deletes all players
 * in the gamestate by calling player_delete on each player.
 * 
 * @param state: gamestate
 */
static void
gamestate_playersDeleteHelper(gamestate_t* state){
  // Get number of players objects
  int numberOfPlayers = state->players_seen;
  
  // Loop over every item in the players array and delete the player
  for(int i = 0; i < numberOfPlayers; i++){
    // Get current player
    player_t* currentPlayer = state->players[i];

    // Run player_delete on current player
    player_delete(currentPlayer);
  }
}

/**
 * @brief: a getter method for the array of players in the gamestate
 * 
 * Inputs:
 * @param game: gamestate
 * 
 * Returns:
 * @return player_t**: an array of pointers to player_t structs
 * 
 * NOTE: The gamestate makes no promise on the actual number of
 * players in the returned array of players.
 * The caller must handle the case where the returned array of
 * players has zero players.
 * 
 * The gamestate marks empty player slots with a NULL flag,
 * and players are added sequentially as they arrive, 
 * from index 0 onwards up to index 25.
 * A plausible way for the caller to check nonexistence
 * of players or count the number of players is to count
 * until the first NULL in the array.
 */
player_t**
gamestate_getPlayers(gamestate_t* state)
{
  return state->players;
}


/**
 * @brief: function to add players to the game.
 *  
 * Inputs:
 * @param state: a pointer to the gamestate 
 * for the current game session.
 * @param player: the player to add to the game.
 * 
 * Returns: None.
 * 
 * NOTE: The game accomodates a maximum of 26 players.
 * If 26 players already joined the game beforehand,
 * a new join request will be denied and the 
 * prospective player will be sent a message that
 * the game is full.
 */
void
gamestate_addPlayer(gamestate_t* state, player_t* player)
{
  /* if gamestate is not NULL */
  if (state != NULL && player != NULL) {

    /* if there's still space for more players */
    if (state->players_seen < 26) {

      /* add new player, increment num of players seen */
      state->players[state->players_seen++] = player;
    }
    else {

      /* if game is full, send QUIT message */
      player_send(player, "QUIT Game is full.");
    }
  }
}


/**
 * @brief: function to add a spectator to the game.
 *  
 * @param state: a pointer to the gamestate 
 * for the current game session.
 * @param address: the address of the spectator to add to the game.
 * 
 * NOTE: The game can only have a single spectator at a time.
 * If there's a spectator in the game, the old spectator will
 * be sent a QUIT message and replaced by the new spectator.
 * 
 * Returns: None
 */
void
gamestate_addSpectator(gamestate_t* state, addr_t address)
{
  /* if game is not NULL */
  if (state != NULL) {

    /* if a spectator exists in game, replace them */
    if (state->spectator != NULL) {
      spectator_send(state->spectator, "QUIT You have been replaced by a new spectator.");
      spectator_delete(state->spectator);
    }

    /* save new spectator */
    state->spectator = spectator_new(address);
  }
}


/**
 * @brief: a function that checks if a given address
 * belongs to the current spectator in the game.
 * 
 * Inputs:
 * @param state: the gamestate for the current session of the game.
 * @param address: address to check (addr_t struct)
 * 
 * Returns:
 * @return true: the address matches the spectator's address.
 * @return false: the address does not match current spectator.
 */
bool
gamestate_isSpectator(gamestate_t* state, addr_t address)
{
  /* defensive checks */
  if (state != NULL && state->spectator != NULL) {

    /* check if address matches that of spectator in game */
    spectator_t* spectator = state->spectator;
    return message_eqAddr(address, spectator->address);
  }

  /* if either is NULL, return false */
  return false;
}


/**
 * @brief: a function to find a player in the game
 * that matches a given address.
 * 
 * @param state: the gamestate for the current session of the game.
 * @param address: the address to check for a match.
 * @return player_t*: the player matching given address.
 * 
 * @return NULL: no matching player was found.
 */
player_t*
gamestate_findPlayerByAddress(gamestate_t* state, addr_t address)
{
  if (state != NULL) {    /* defensive check */

    /* loop through players
       checking for a matching address */
    for (int i = 0; i < state->players_seen; i++) {
      player_t* player = state->players[i];
      if (message_eqAddr(address, player->address)) {
        return player;
      }
    }

    /* if no player found, return NULL */
    fprintf(stderr, "No player found matching given address.\n");
    return NULL;
  }

  /* if gamestate is NULL, 
     print error message and return NULL */
  fprintf(stderr, "Attempt to find player in NULL gamestate. Stop.\n");
  return NULL;
}

/**
 * @brief: function to close the gamestate tracker
 * for a game instance.
 * 
 * @param state: the gamestate for the current 
 * instance of the game.
 */
void gamestate_closeGame(gamestate_t* state){
  // Close grid
  grid_delete(state->masterGrid);

  // Close spectator
  spectator_delete(state->spectator);
  
  // Close players array
  gamestate_playersDeleteHelper(state);

  // Close gold
  gold_delete(state->gameGold);

  // Free entire gamestate object
  free(state);
}