#!/bin/bash
#
# testing.sh
# usage:
#   ./testing.sh
# output:
#   stdout
#
# Author: TEAM PINE, May 2021

VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"


: '
Testing Script for nuggets game.
'

# build game
printf "Building game...\n"
make -C ../

# incorrect usage

# no args (too few)
$VALGRIND ../server

# three args (too many)
$VALGRIND ../server ../maps/main.txt 257573 dummy

# invalid map file
$VALGRIND../server ./nomap.txt 257573

# invalid seed value
$VALGRIND ../server ../maps/main.txt WRONGSEED

# valid call

## launch server
$VALGRIND ../server ../maps/main.txt 257573
