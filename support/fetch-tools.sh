#!/usr/bin/env bash

if   which curl; then FETCH='curl -O';
elif which wget; then FETCH='wget -c';
else exit;
fi

${FETCH} http://aichallenge.org/tools.tar.bz2
tar xjf tools.tar.bz2
rm -f tools.tar.bz2
