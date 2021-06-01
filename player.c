/**
 * @file player.c
 * @author TEAM PINE
 * @brief: defines functionality for the player module.
 * The player module offers abstractiona and interface
 * for the players in the game.
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

#include "message.h"  /* message module */
#include "grid.h"     /* grid module */
#include "player.h"   /* self */


/**
 * @brief: constructor
 * 
 * Inputs:
 * @param letter: the player's letter.
 * @param name: the player's name.
 * @param address: the player's address (addr_t)
 * @param x: the player's starting x coordinate.
 * @param y: the player's starting y coordinate.
 * @param grid: a grid instance to track the player's
 * view of the world.
 * 
 * Returns:
 * @return player_t*: pointer to a player struct 
 * representing the data passed in.
 * 
 * NOTE: this function allocates memory for the player struct.
 * The caller must later free that pointer by calling player_delete().
 */
player_t* 
player_new(char letter, char* name, addr_t address, int x, int y, grid_t* grid) 
{
  /* allocate memory for struct */
  player_t* player = malloc(sizeof(player_t));

  /* save data in struct */
  player->letter = letter;
  player->name = malloc(sizeof(name));
  strcpy(player->name, name);

  player->address = address;
  player->gold = 0;
  player->x = x;
  player->y = y;
  player->grid = grid;

  /* return pointer to player struct */
  return player;
}

/**
 * @brief: this function allocates a given number of gold
 * to the specified player.
 * 
 * Inputs:
 * @param player: pointer to player struct.
 * @param numGold: number of gold to assign.
 */
void
player_addGold(player_t* player, int numGold)
{
  /* cannot assign gold to NULL player */
  if (player != NULL) {
    player->gold += numGold;
  }
  /* print error message IF player is NULL */
  fprintf(stderr, "Attempt to assign gold to NULL player. Stop.\n");
}

/**
 * @brief: this function finds the number of gold
 * currently held by a player.
 * 
 * Inputs:
 * @param player: pointer to a player struct
 * 
 * Returns:
 * @return int: integer value of total gold collected.
 * NOTE: we return -1 in case of a NULL player.
 * The caller may check for that condition as an error flag.
 */
int
player_getGold(player_t* player)
{
  /* cannot get gold from NULL player. */
  if (player != NULL) {
    return player->gold;
  }
  /* print error message if NULL player */
  fprintf(stderr, "Attempt to get gold from NULL player. Stop.\n");
  return -1;
}


/**
 * @brief: this function moves a player by specified values dx and dy.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * @param dx: delta x by which to move player.
 * @param dy: delta y by which to move player.
 * NOTE: we do not check the validity of the move.
 * The caller is expected to have checked for 
 * the validity of the move before calling this function.
 * 
 * Returns: None.
 */
void
player_move(player_t* player, int dx, int dy)
{
  /* cannot move NULL player */
  if (player != NULL) {
    player->x += dx;
    player->y += dy;
    return;
  }
  /* if NULL player, print error message. */
  fprintf(stderr, "Attempt to move NULL player. Stop.\n");
}


/**
 * @brief: this function finds the letter assigned to a given player.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * 
 * Returns:
 * @return char: character assigned to player.
 */
char
player_getLetter(player_t* player)
{
  /* cannot get letter of NULL player */
  if (player != NULL) {
    return player->letter;
  }
  /* if player is NULL, print error message */
  fprintf(stderr, "Attempt to get letter of NULL player. Stop.\n");
  return -1;
}


/**
 * @brief: this function finds the name assigned to a given player.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * 
 * Returns:
 * @return char*: string name of player.
 * NOTE: this function does not assign fresh memory for the player's name.
 * The caller may NOT free the returned pointer.
 */
char*
player_getName(player_t* player)
{
  /* cannot get name of NULL player */
  if (player != NULL) {
    return player->name;
  }
  /* print error message if player is NULL, return NULL */
  fprintf(stderr, "Attempt to get name of NULL player. Stop.\n");
  return NULL;
}


/**
 * @brief: this function finds the x coordinate of a given player.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * 
 * Returns:
 * @return int: x coordinate of player.
 */
int
player_getX(player_t* player)
{
  if (player != NULL) {
    return player->x;
  }
  fprintf(stderr, "Attempt to get coordinates of NULL player. Stop.\n");
  return -1;
}


/**
 * @brief: this function finds the y coordinate of a given player.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * 
 * Returns:
 * @return int: y coordinate of player.
 */
int
player_getY(player_t* player)
{
  if (player != NULL) {
    return player->y;
  }
  fprintf(stderr, "Attempt to get coordinates of NULL player. Stop.\n");
  return -1;
}


/**
 * @brief: function to send a message to a given player.
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * @param message: message (char*) to send.
 * 
 * Returns: None
 * 
 * NOTE: we do not free any of the pointers passed in.
 */
void
player_send(player_t* player, char* message)
{
  if (player != NULL && message != NULL) {
    message_send(player->address, message);
  }
}


/**
 * @brief: function to delete a player struct 
 * and free all memory allocated during creation.
 * 
 * NOTE: it is assumed that the struct was created
 *  via a call to player_new()
 * 
 * Inputs:
 * @param player: pointer to a player struct.
 * 
 * Returns: None
 */
void
player_delete(player_t* player)
{
  if (player != NULL) {
    // Free player associated grid
    grid_delete(player->grid);
    
    // Free player name
    free(player->name);

    // Free entire player object
    free(player);
  }
}
