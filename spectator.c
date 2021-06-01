/**
 * @file spectator.h
 * @author TEAM PINE
 * @brief: defines functionality for the spectator module.
 * The spectator module offers abstraction and interface
 * with a spectator in the game, allowing them to view 
 * the full state of the game. 
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>

#include "message.h"      /* message module */
#include "spectator.h"    /* self */

/**
 * @brief: constructor: allocates memory for the spectator struct
 * and initializes values
 * 
 * Inputs:
 * @param address: spectator's address (addr_t)
 * NOTE: this function allocates memory for the new spectator struct.
 * The callse is expected to later free the returned
 * pointer via a call to spectator_delete()
 * 
 * Returns:
 * @return spectator_t*: pointer to a spectator struct.
 */
spectator_t* 
spectator_new(addr_t address) 
{
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator != NULL) {
    spectator->address = address;
    return spectator;
  }
  /* if error occurred allocating memory, 
     print error message and return NULL */
  fprintf(stderr, "Error allocating memory for spectator.");
  return NULL;
}


/**
 * @brief: function to send a message to a spectator
 * 
 * Inputs:
 * @param spectator: pointer to a spectator struct
 * @param message: string message to send (char*)
 * 
 * Returns: None.
 */
void
spectator_send(spectator_t* spectator, char* message)
{
  /* cannot send NULL message, 
     or to NULL spectator */
  if (spectator != NULL && message != NULL) {
    message_send(spectator->address, message);
  }
}


/**
 * @brief: function to delete a spectator instance
 * and free all allocated memory.
 * 
 * NOTE: It's assumed that the pointer was initialized 
 * via a call to spectator_new()
 * 
 * Inputs:
 * @param spectator: pointer to a spectator struct
 * 
 * Returns: None
 */
void
spectator_delete(spectator_t* spectator)
{
  /* cannot free NULL spectator */
  if (spectator != NULL) {
    free(spectator);
  }
}
