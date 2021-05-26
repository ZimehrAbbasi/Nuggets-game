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

typedef struct game {
  grid_t* master;               /* master grid */
  spectator_t* spectator;       /* single spectator -- is NULL if no spectator in game */ 
  
  player_t* players[26];        /* array of players */
  player_t* prevplayers[26];    /* track previous players in the game */
  int players_seen;             /* track players seen -- whether in game or left */
  grid_t* playerGrids[26];      /* player grids */
} gamestate_t;

gamestate_t*
gamestate_init(char* mapFile)
{
  gamestate_t* state = malloc(sizeof(gamestate_t));
  if (state == NULL) {
    fprintf(stderr, "Error allocating memory for gamestate.\n");
    return NULL;
  }

  FILE* fp = fopen(mapFile);
  state->master = grid_init(mapFile, )

}

gamestate_t**
gamestate_getPlayers(gamestate_t* game)
{
  return game->players;
}

void
gamestate_addSpectator(gamestate_t* state, spectator_t* newSpectator)
{
  if (state != NULL && newSpectator != NULL) {
    if (state->spectator != NULL) {
      spectator_delete(state->spectator);
    }
    state->spectator = newSpectator;
  }
}