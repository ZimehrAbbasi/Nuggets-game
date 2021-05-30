/**
 * @file player.c
 * @author TEAM PINE
 * @brief: functionality for the spectator module
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

#include "message.h"    /* message module */
#include "spectator.h"  /* self */

spectator_t* 
spectator_new(addr_t address) 
{
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator != NULL) {
    spectator->address = address;
    return spectator;
  }
  fprintf(stderr, "Error allocating memory for spectator.");
  return NULL;
}

void
spectator_send(spectator_t* spectator, char* message)
{
  if (spectator != NULL && message != NULL) {
    message_send(spectator->address, message);
  }
}

void
spectator_delete(spectator_t* spectator)
{
  if (spectator != NULL) {
    free(spectator);
  }
}
