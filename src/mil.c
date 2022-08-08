/*
** Mil - A small and concatenative programming language. Implemented in C99.
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
#include <unistd.h>

// == Tokenize functions + constants ==
#define number(c)     ( c >= '0' && c <= '9' )
#define whitespace(c) ( c == ' ' || c == '\t' || c == '\f' )
#define STACK_SIZE    ( 4096 )
#define REG_SIZE      ( 256  )

int STACK[STACK_SIZE];  // Stack
int REG[REG_SIZE];      // General registers (for storing numbers)
char* FUNC[REG_SIZE];   // Function registers (for storing functions)
int stc = -1;           // Stack counter
FILE* fptr;             // File pointer for reading file
char* buffer;           // File content buffer
long nb;                // Number of bytes read from file

// == Stack + Helper functions ==
void push(int n)    { STACK[++stc] = n; }
void pop()          { STACK[stc--] = 0; }
int peek()          { return STACK[stc];}
int pop_return()    { int i = peek(); pop(); return i; }
void pop_print()    { printf("%d\n", pop_return()); }
void debug_stack()  { for (int i = 0; i <= stc; i++) printf("STACK %d: %d\n", i, STACK[i]); }
void die(const char* msg) {
  perror(msg);
  exit(64);
}

// == Parsing + Running (Eval) ==
void eval(char* code) {
  // Token counter (tc) and Line counter (lc)
  size_t tc = 0, lc = 1;
  while (tc < strlen(code)) {
    char current = code[tc];
    if (whitespace(current)) {
      // Skip whitespace
      tc++;
    } else if (current == '\n') {
      lc++; tc++;
    } else if (current == '#') {
      // Skip comments
      while (tc < strlen(code) && code[tc] != '\n') tc++;
    } else if (number(current)) {
      /* We will only eval integers at the moment.
       * No hex, no octal. etc. */
      int value = current - '0';
      if (value > 0) {
        // Parse decimal number
        // From https://github.com/lotabout/write-a-C-interpreter/blob/master/xc-tutor.c#L108,L110 
        // Reset token value to avoid first number duplication (e.g. 6 -> 66)
        value = 0;
        while (number(code[tc]))
          value = value * 10 + code[tc++] - '0';
      }
      printf("NUMBER (%d) at %d, line %d.\n", value, tc, lc);
      push(value);
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
    } else {
      printf("TOKEN (%c) at %d, line %d.\n", current, tc, lc);
      switch (current) {
        // Arithmetic and comparison operators
        case '+': push(pop_return() + pop_return()); tc++; break;
        case '-': push(pop_return() - pop_return()); tc++; break;
        case '*': push(pop_return() * pop_return()); tc++; break;
        case '/': push(pop_return() / pop_return()); tc++; break;
        case '%': push(pop_return() % pop_return()); tc++; break;
        case '=': push(pop_return() == pop_return()); tc++; break;
        case '!': (code[tc++] == '=' ? push(pop_return() != pop_return()) : push(!(pop_return()))); break;
        case '<': (code[tc++] == '=' ? push(pop_return() <= pop_return()) : push(pop_return() < pop_return())); break;
        case '>': (code[tc++] == '=' ? push(pop_return() >= pop_return()) : push(pop_return() > pop_return())); break;
        // Printing operators
        case '$': {
					for (int i = 0; i <= stc; i++)
					  printf("%c", STACK[i]);
					printf("\n");
					tc++;
					break;
        }
        case 'v': debug_stack(); tc++; break;
        case '.': pop_print(); tc++; break;
        case '@': printf("%d\n", peek()); tc++; break;
        // Stack control operators
        case 'd': {
          push(peek());
          tc++;
					break;
        }
        case 'c': {
					for (int i = 0; i <= stc; i++)
					  STACK[i] = 0;
					stc = -1;
					tc++;
					break;
        }
        case 'r': {
					int a = pop_return(), b = pop_return();
					push(a);
					push(b);
					tc++;
					break;
        }
        // Store and load variables
        case 's': REG[(int)code[tc++]] = pop_return(); break;
        case 'l': push(REG[(int)code[tc++]]); break;
        // Eval and functions
        case 'x': {
          char expr[4096] = "";
          /* A pretty clever way to concat a char with a string
          * Here, we write the formatted output `%s%c` to expr
          * with %s containing expr itself and %c containing the
          * character at the STACK[i]
          */
          for (int i = 0; i <= stc; i++)
            sprintf(expr, "%s%c", expr, (char)STACK[i]);
          eval(expr);
          tc++;
          break;
        }
        // Input
        case 'i': {
          int i = 0;
          scanf("%d", &i);
          push(i);
          tc++;
          break;
        }
        default: printf("UNKNOWN (%c)\n", current); tc++; break;
      }
    }
  }
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
  // ASCII art text. Might look pretty funky but renders really well
  const char* text =
    "     ___                       ___\n"
    "    /\\__\\          ___        /\\__\\\n"
    "   /::|  |        /\\  \\      /:/  /\n"
    "  /:|:|  |        \\:\\  \\    /:/  /\n"
    " /:/|:|__|__      /::\\__\\  /:/  /  \n"
    "/:/ |::::\\__\\  __/:/\\/__/ /:/__/   \n"
    "\\/__/~~/:/  / /\\/:/  /    \\:\\  \\   \n"
    "      /:/  /  \\::/__/      \\:\\  \\  \n"
    "     /:/  /    \\:\\__\\       \\:\\  \\ \n"
    "    /:/  /      \\/__/        \\:\\__\\\n"
    "    \\/__/                     \\/__/\n";
  printf("%s", text);
  printf("Mil is a small and concatenative programming language implemented in C99.\n");
  printf("Version %s\n", MIL_VERSION);
  printf("Usage: mil <src>\n");
}

// == Main ==
int main(int argc, char* argv[]) {
  if (argc > 2) {
    help();
  } else if (argc == 2) {
    eval(readsrc(argv[1]));
  } else {
    printf("REPL not supported (yet)\n");
    help();
  }
  return 0;
}
