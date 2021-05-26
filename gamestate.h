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

typedef struct game gamestate_t;

gamestate_t* gamestate_init(FILE* mapFile);

gamestate_t** gamestate_getPlayers(gamestate_t* game);

#endif