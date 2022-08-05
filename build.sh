#!/bin/bash
gcc src/mil.c -DFE_STANDALONE -O3 -o mil -Wall -Wextra -std=c99 -pedantic -g
