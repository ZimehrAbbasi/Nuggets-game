/**
 * @file gamestate.h
 * @author TEAM PINE
 * @brief: exports functionality for the gamestate module.
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

#ifndef __GAMESTATE_H
#define __GAMESTATE_H

#include <stdio.h>
#include <stdlib.h>

#include "player.h"     /* player module */
#include "grid.h"       /* grid module */
#include "gold.h"       /* gold module */
#include "spectator.h"  /* spectator module */

/**
 * @brief: The gamestate_t* struct tracks the state of the game, 
 * including the players, spectators, gold, master and player grids,
 * and the total number of players seen. 
 */
typedef struct game {
  grid_t* masterGrid;           /* master grid */
  spectator_t* spectator;       /* single spectator -- is NULL if no spectator in game */ 
  player_t* players[26];        /* array of players */
  int players_seen;             /* track players seen -- whether in game or left */
  gold_t* gameGold;             /* keep track of gold in the game */
} gamestate_t;


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
gamestate_t* gamestate_init(FILE* mapFile);


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
player_t** gamestate_getPlayers(gamestate_t* game);


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
bool gamestate_isSpectator(gamestate_t* state, addr_t address);


/**
 * @brief: a function to find a player in the game
 * that matches a given address.
 * 
 * Inputs:
 * @param state: the gamestate for the current session of the game.
 * @param address: the address to check for a match.
 * 
 * Returns:
 * @return player_t*: the player matching given address.
 * @return NULL: no matching player was found.
 */
player_t* gamestate_findPlayerByAddress(gamestate_t* state, addr_t address);


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
void gamestate_addPlayer(gamestate_t* state, player_t* player);


/**
 * @brief: function to add a spectator to the game.
 *  
 * Inputs:
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
void gamestate_addSpectator(gamestate_t* game, addr_t address);


/**
 * @brief: function to close the gamestate tracker
 * for a game instance and free all malloc'd memory.
 * 
 * Inputs:
 * @param state: the gamestate for the current 
 * instance of the game.
 * 
 * Returns: None
 */
void gamestate_closeGame(gamestate_t* state);

#endif