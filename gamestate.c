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

gamestate_t**
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