# Nuggets

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

***


// GRADER: where is the list of team members?

## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.

// GRADER what about new directory `tests/`?

***

## Compiling

To build, run `make`.

To test with outomated players, run `make test`.

To clean up, run `make clean`.

To test for memory leaks, run `make memcheck`. Note: This requires you to either manually add bots to the game or call [./tests/runbots.sh](./tests/runbots.sh) with the port number that the server instance returned.
