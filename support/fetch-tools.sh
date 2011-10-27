#!/usr/bin/env bash

if   which curl; then FETCH='curl -O';
elif which wget; then FETCH='wget -c';
else exit
fi

if [ "$OSTYPE" == "msys" ]; then
  PACKAGE='tools.zip';
else
  PACKAGE='tools.tar.bz2';
fi

${FETCH} "http://aichallenge.org/${PACKAGE}"

if [ "$OSTYPE" == "msys" ]; then
  unzip ${PACKAGE}
else
  tar xjf ${PACKAGE}
fi
rm -f "${PACKAGE}"
