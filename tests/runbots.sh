#!/bin/bash
#
# runbots.sh
# usage:
#   ./runbots.sh [PORT]
# output:
#   stdout
#
# Author: TEAM PINE, May 2021


# add a spectator
# echo "Adding spectator... to port $@"
# prof/player localhost $@ 2> /dev/null &

# add 10 background bots

for i in {1..10}; do

  # add new bot
  linux/player localhost $1 botbg &> /dev/null &

  echo "Added bot number $i!"
done

# add a spectator
echo "Adding spectator... to port $1" &
linux/player localhost $1 2> /dev/null
