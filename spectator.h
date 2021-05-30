/**
 * @file player.h
 * @author TEAM PINE
 * @brief: functionality for the spectator module
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __SPECTATOR_H
#define __SPECTATOR_H

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

#include "message.h"  /* message module */

typedef struct spectator {
  addr_t address;
} spectator_t;

spectator_t* spectator_new(addr_t address);

void spectator_send(spectator_t* spectator, char* message);

void spectator_delete(spectator_t* spectator);

#endif /* __SPECTATOR_H */