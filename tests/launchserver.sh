#!/bin/bash
#
# launchserver.sh
# usage:
#   ./launchserver.sh [mapfile] [seed value]
# output:
#   returns value
#
# Author: TEAM PINE, May 2021

function runserver()
{
  local __mapfile=$1
  local __seedval=$2
  ../server $__mapfile $__seedval &> temp.out &
  sleep 3
  local portnum=`sed -n '${s/.* //; p}' temp.out`
  echo "$portnum"
  sed -e "s/^\'//" -e "s/\'$//" <<< "$portnum"
  # echo "$portnum"
}

runserver "$@"

