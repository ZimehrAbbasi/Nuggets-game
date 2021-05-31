/**
 * @file player.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __PLAYER_H
#define __PLAYER_H
/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

/* message module */
#include "support/message.h"
#include "grid.h"

typedef struct player {
  char letter;
  char* name;
  addr_t address;
  int gold;
  int x;
  int y;
  grid_t* grid;
} player_t;


player_t* player_new(char letter, char* name, addr_t address, int x, int y, grid_t* grid);

void player_send(player_t* player, char* message);

char player_getLetter(player_t* player);

void player_addGold(player_t* player, int num);

int player_getGold(player_t* player);

void player_move(player_t* player, int dx, int dy);

int player_getX(player_t* player);

int player_getY(player_t* player);

void player_delete(player_t* player);

#endif /* __PLAYER_H */