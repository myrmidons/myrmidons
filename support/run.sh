#!/usr/bin/env bash

ROOT=`dirname $0`
echo $ROOT
PATH=${ROOT}:${PATH}

PYTHON=`which python`

if [ -z "${PYTHON}" ]; then
  echo "python not found, aborting"
  exit 1
fi

if [ ! -d "../tools" ]; then
  echo "../tools not found"
  exit 2
fi

if [ -e "MyBot" ]; then
  BOT="./MyBot"
elif [ -e "debug/bot" ]; then
  BOT="debug/bot"
elif [ -e "release/bot" ]; then
  BOT="release/bot"
elif [ -e "bot" ]; then
  BOT="bot"
fi

NETCAT=`which nc netcat`

P1="nc localhost 19001"
P1="sh -c 'cat > foo.txt'"
P2="python ../tools/sample_bots/python/HunterBot.py"
MAPFILE="../tools/maps/example/tutorial1.map"

${PYTHON} ../tools/playgame.py \
         "${P1}" \
         "${P2}" \
         --map_file ${MAPFILE} \
         --log_dir game_logs \
         --turns 1000 \
         --scenario \
         --player_seed 7 \
         --verbose \
         -e
