/*
** Copyright (c) 2022-present Dang Hoang Tuan (Tsuki) <tsukii@disroot.org>
**
** This library is free software; you can redistribute it and/or modify it
** under the terms of the MIT license. See `mil.c` for details.
*/

#ifndef MIL_H
#define MIL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MIL_VERSION "1.0"

// == Helper functions ==
int is_number(char c);
int is_identifier(char c);
int is_whitespace(char c);
void die(const char* msg);
// == Stack functions ==
void push(int n);
void pop();
int peek();
int pop_return();
void pop_print();
void debug_stack();
// == Parsing + Running ==
void next();
void eval();
// == Reading source code from files ==
char* readsrc(const char* filename)
// == Printing help ==
void help();

#endif
