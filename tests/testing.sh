#!/bin/bash
#
# testing.sh
# usage:
#   ./testing.sh
# output:
#   stdout
#
# Author: TEAM PINE, May 2021

: '
Testing Script for nuggets game.
'

# build game
printf "Building game...\n"
make -C ../

# incorrect usage

# no args (too few)
../server

# three args (too many)
../server ../maps/main.txt 257573 dummy

# invalid map file
../server ./nomap.txt 257573

# invalid seed value
../server ../maps/main.txt WRONGSEED

# valid call

## launch server
port=$(./launchserver.sh ../maps/main.txt 257573)
port="${port%\'}"
port="${port#\'}"
parsed=`tr -d "\'" <<< $port`
# sed -e "s/^\'//" -e "s/\'$//" <<< "$port"
printf "received port: $parsed \n"

./runbots.sh $parsed

# ../server ../maps/main.txt 257573


