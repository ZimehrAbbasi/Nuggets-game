/**
 * @file player.c
 * @author your name (you@domain.com)
 * @brief: functionality for the player module
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

#include "player.h" /* player module */
#include "message.h"  /* message module */
#include "grid.h"     /* grid module */


player_t* 
player_new(char letter, char* name, addr_t address, int x, int y, grid_t* grid) 
{
  player_t* player = malloc(sizeof(player_t));

  player->letter = letter;
  player->name = malloc(sizeof(name));
  strcpy(player->name, name);

  player->address = address;
  player->gold = 0;
  player->x = x;
  player->y = y;
  player->grid = grid;
}

void
player_send(player_t* player, char* message)
{
  message_send(player->address, message);
}

void
player_addGold(player_t* player, int numGold)
{
  if (player != NULL) {
    player->gold += numGold;
  }
}

int
player_getGold(player_t* player)
{
  if (player != NULL) {
    return player->gold;
  }
  fprintf(stderr, "Attempt to get gold from NULL player. Stop.\n");
  return -1;
}

void
player_move(player_t* player, int dx, int dy)
{
  if (player != NULL) {
    player->x += dx;
    player->y += dy;
    return;
  }
  fprintf(stderr, "Attempt to move NULL player. Stop.\n");
}

char
player_getLetter(player_t* player)
{
  if (player != NULL) {
    return player->letter;
  }
  fprintf(stderr, "Attempt to get letter of NULL player. Stop.\n");
  return -1;
}

char*
player_getName(player_t* player)
{
  if (player != NULL) {
    return player->name;
  }
  fprintf(stderr, "Attempt to get name of NULL player. Stop.\n");
  return NULL;
}

int
player_getX(player_t* player)
{
  if (player != NULL) {
    return player->x;
  }
  fprintf(stderr, "Attempt to get coordinates of NULL player. Stop.\n");
  return -1;
}

int
player_getY(player_t* player)
{
  if (player != NULL) {
    return player->y;
  }
  fprintf(stderr, "Attempt to get coordinates of NULL player. Stop.\n");
  return -1;
}

void
player_send(player_t* player, char* message)
{
  if (player != NULL && message != NULL) {
    message_send(player->address, message);
  }
}


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

void
player_updateGrid(player_t* player, grid_t* grid)
{
  if (player != NULL && grid != NULL) {
    
  }
}
