#!/bin/bash
gcc src/nil.c -DFE_STANDALONE -O3 -o nil -Wall -Wextra -std=c99 -pedantic -g
