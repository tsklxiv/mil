/*
** Mil - A simple stack-based programming language. Written in C.
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

#include "mil.h"

int STACK[8092];        // Stack
int stc = -1;           // Stack counter
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
void die(const char* msg) {
  perror(msg);
  exit(64);
}
// == Stack functions ==
void push(int n)    { STACK[++stc] = n; }
void pop()          { STACK[stc--] = 0; }
int peek()          { return STACK[stc];}
int pop_return()    { int i = peek(); pop(); return i; }
void pop_print()    { printf("%d\n", pop_return()); }
void debug_stack()  { for (int i = 0; i <= stc; i++) printf("STACK %d: %d\n", i, STACK[i]); }

// == Parsing + Running ==
void next() {
  while (tc < strlen(code) + 1) {
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
      /* We will only eval integers at the moment.
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
      push(tokval);
      return;
    } else if (current == '"') {
      tc++;
      while (code[tc] != '"') {
        char ctc = code[tc++];
        // Report error if we reach to the EOL or the EOF
        // and still haven't find the closed quoting
        if (ctc == '\n' || ctc == '\0')
          die("Unclosed string literal");
        push(ctc);
      }
      tc++; // Avoid including the part that isn't a string
      return;
    } else {
      printf("TOKEN (%c) at %d, line %d.\n", current, tc, lc);
      tc++;
      switch (current) {
        case '+': push(pop_return() + pop_return()); tc++; return;
        case '-': push(pop_return() - pop_return()); tc++; return;
        case '*': push(pop_return() * pop_return()); tc++; return;
        case '/': push(pop_return() / pop_return()); tc++; return;
        case '%': push(pop_return() % pop_return()); tc++; return;
        case '=': push(pop_return() == pop_return()); tc++; return;
        case '!': (code[tc++] == '=' ? push(pop_return() != pop_return()) : push(!(pop_return()))); return;
        case '<': (code[tc++] == '=' ? push(pop_return() <= pop_return()) : push(pop_return() < pop_return())); return;
        case '>': (code[tc++] == '=' ? push(pop_return() >= pop_return()) : push(pop_return() > pop_return())); return;
        case '$': {
          for (int i = 0; i <= stc; i++)
            printf("%c", STACK[i]);
          printf("\n");
          tc++;
          return;
        }
        case 'v': debug_stack(); tc++; return;
        case '.': pop_print(); tc++; return;
        case '@': printf("%d\n", peek()); tc++; return;
        case 'd': {
          push(peek());
          tc++;
          return;
        }
        case 'c': {
          for (int i = 0; i <= stc; i++)
            STACK[i] = 0;
          stc = -1;
          tc++;
          return;
        }
        default: printf("UNKNOWN (%c)\n", current); tc++; return;
      }
    }
  }
}

void eval() {
  while (tc < strlen(code)) {
    next();
  }
  debug_stack();
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

// == Printing help ==
void help() {
  printf("Mil is a small and concatenative programming language written in C.\n");
  printf("Version %s\n", MIL_VERSION);
  printf("Usage: mil <src>\n");
}

// == Main ==
int main(int argc, char* argv[]) {
  if (argc > 2) {
    help();
  } else if (argc == 2) {
    code = readsrc(argv[1]);
    eval();
  } else {
    printf("REPL not supported (yet)\n");
    help();
  }
  return 0;
}
