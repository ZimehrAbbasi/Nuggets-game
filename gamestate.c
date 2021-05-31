/**
 * @file gamestate.c
 * @author TEAM PINE
 * @brief: functionality for gamestate
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "spectator.h"
#include "grid.h"
#include "gold.h"
#include "spectator.h"
#include "gamestate.h"

static void gamestate_initPlayers(gamestate_t* state);
static void gamestate_initGold(gamestate_t* state);
static void gamestate_initGrid(gamestate_t* state, FILE* mapFile);
static void gamestate_initSpectator(gamestate_t* state);
static void gamestate_playersDeleteHelper(gamestate_t* state);

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

static void 
gamestate_initPlayers(gamestate_t* state){
  *(state->players) = malloc(sizeof(player_t) * 26);
}

static void 
gamestate_initGold(gamestate_t* state){
  state->gameGold = gold_init(26);
}

static void
gamestate_initGrid(gamestate_t* state, FILE* mapFile){
  state->masterGrid = grid_init(mapFile);
}

static void
gamestate_initSpectator(gamestate_t* state){
  state->spectator = NULL;
}

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

player_t**
gamestate_getPlayers(gamestate_t* game)
{
  return game->players;
}

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

void
gamestate_addSpectator(gamestate_t* game, addr_t address)
{
  /* if game is not NULL */
  if (game != NULL) {

    /* if a spectator exists in game, replace them */
    if (game->spectator != NULL) {
      spectator_send(game->spectator, "QUIT You have been replaced by a new spectator.");
      spectator_delete(game->spectator);
    }

    /* save new spectator */
    game->spectator = spectator_new(address);
  }
}

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