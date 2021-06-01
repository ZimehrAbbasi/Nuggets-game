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
echo "Adding spectator... to port $@"
prof/player localhost $@ 2> /dev/null &

# add 25 background bots
for i in {1..10}; do

  # add new bot
  prof/player localhost $@ botbg &> /dev/null &

  echo "Added bot number $i!"
done

# add a spectator
# echo "Adding spectator..." &
prof/player localhost $@ 2> /dev/null


