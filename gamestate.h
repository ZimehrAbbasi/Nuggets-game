/**
 * @file gamestate.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __GAMESTATE_H
#define __GAMESTATE_H

#include "spectator.h"
#include "gold.h"

typedef struct game {
  grid_t* masterGrid;           /* master grid */
  spectator_t* spectator;       /* single spectator -- is NULL if no spectator in game */ 
  player_t* players[26];        /* array of players */
  int players_seen;             /* track players seen -- whether in game or left */
  gold_t* gameGold;             /* keep track of gold in the game */
} gamestate_t;

gamestate_t* gamestate_init(FILE* mapFile);

gamestate_t** gamestate_getPlayers(gamestate_t* game);

/*
* Closes the game and frees all malloc'd memory in the gamestate object
*/
void gamestate_closeGame(gamestate_t* state);

#endif