/*
** Nil - A simple stack-based programming language. Written in C.
** Copyright (c) 2022 Dang Hoang Tuan (Tsuki) <tsukii@disroot.org>
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

// == Tokens ==
enum Token {
  // Single-character tokens (operators in precedence order)
  Not, And, Or, Eq, Lt, Gt, Add, Sub, Mul, Div, Mod,
  // Two-character tokens
  Ne, Lte, Gte, Inc, Dec,
  // Literals
  Ident, Str, Num,
  // Keywords
  Print
};

u_int16_t STACK[8092];  // Stack
u_int16_t stc = 0;      // Stack counter
size_t tc = 0, lc = 1;  // Token counter (tc) and Line counter (lc)
int token;              // Current token
int tokval;             // Token value (mainly for dealing with numbers)
FILE* fptr;             // File pointer for reading file
char* buffer;           // File content buffer
long nb;                // Number of bytes read from file
char* code;             // Source code

// == Helper functions ==
int is_number(char c)     { return c >= '0' && c <= '9'; }
int is_identifier(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'); }
int is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\f'; }
// == Stack functions ==
void push(u_int16_t n)    { STACK[++stc] = n; }
void pop()                { STACK[stc--] = 0; }
u_int16_t peek()          { return STACK[stc];}
void die(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

// == Parsing ==
void next() {
  while (tc < strlen(code)) {
    char current = code[tc];
    if (is_whitespace(current)) {
      // Skip whitespace
      tc++;
    } else if (current == '\n') {
      lc++; tc++;
    } else if (current == '#') {
      // Skip comments
      while (tc < strlen(code) && code[tc] != '\n') tc++;
    } else if (is_number(current)) {
      /* We will only parse integers at the moment.
       * No hex, no octal. etc. */
      tokval = current - '0';
      if (tokval > 0) {
        // Parse decimal number
        // From https://github.com/lotabout/write-a-C-interpreter/blob/master/xc-tutor.c#L108,L110 
        // Reset token value to avoid first number duplication (e.g. 6 -> 66)
        tokval = 0;
        while (is_number(code[tc]))
          tokval = tokval * 10 + code[tc++] - '0';
      }
      printf("NUMBER (%d) at %d, line %d.\n", tokval, tc, lc);
      token = Num;
      return;
    } else {
      printf("TOKEN (%c) at %d, line %d\n", current, tc, lc);
      switch (current) {
        case '=': token = Eq; tc++; return;
        case '!': token = (code[tc++] == '=' ? Ne : Not); tc++; return;
        case '|': token = Or; tc++; return;
        case '&': token = And; tc++; return;
        case '<': token = (code[tc++] == '=' ? Lte : Lt); tc++; return;
        case '>': token = (code[tc++] == '=' ? Gte : Gt); tc++; return;
        case '+': token = (code[++tc] == '+' ? Inc : Add); tc++; return;
        case '-': token = (code[tc++] == '-' ? Dec : Sub); tc++; return;
        case '*': token = Mul; tc++; return;
        case '/': token = Div; tc++; return;
        case '%': token = Mod; tc++; return;
        case '.': token = Print; tc++; return;
        default: printf("UNKNOWN (%c)\n", current); tc++; return;
      }
    }
  }
}

void parse() {
  while (tc < strlen(code)) {
    next();
    switch (token) {
      case Num: push((u_int16_t)tokval); break;
      case Add: {
        int a = peek(); pop();
        int b = peek(); pop();
        printf("%d %d %d\n", a, b, stc);
        push(a + b);
        break;
      }
      case Print: printf("%d", peek()); pop(); break;
      default: break;
    }
  }
  for (int i = 0; i < stc; i++)
    printf("STACK %d: %d\n", i, STACK[i]);
}

// == Reading source code from files ==
char* readsrc(const char* filename) {
  // https://www.fundza.com/c4serious/fileIO_reading_all/index.html
  if ((fptr = fopen(filename, "r")) == NULL)
    die("Error when reading file. Please make sure the file exists.");
  // Get the number of bytes
  fseek(fptr, 0L, SEEK_END);
  nb = ftell(fptr);
  // Then reset the file position indicator to the beginning of the file
  fseek(fptr, 0L, SEEK_SET);
  buffer = (char*)calloc(nb, sizeof(char));
  if (buffer == NULL)
    die("Memory error when allocating buffer for reading file. Please try again.");
  // Copy all the content of file to buffer, then close it
  fread(buffer, sizeof(char), nb, fptr);
  fclose(fptr);
  return buffer;
}

// == Main ==
int main(int argc, char* argv[]) {
  if (argc > 2) {
    printf("Usage: nil <src>");
  } else if (argc == 2) {
    code = readsrc(argv[1]);
    parse();
  } else {
    printf("REPL not supported (yet)");
  }
  return 0;
}
