<!-- markdownlint-configure-file {
  "MD013": {
    "code_blocks": false,
    "tables": false
  },
  "MD033": false,
  "MD041": false
} -->

<div align="center">

```

      ___                       ___ 
     /\__\          ___        /\__\
    /::|  |        /\  \      /:/  /
   /:|:|  |        \:\  \    /:/  / 
  /:/|:|__|__      /::\__\  /:/  /  
 /:/ |::::\__\  __/:/\/__/ /:/__/   
 \/__/~~/:/  / /\/:/  /    \:\  \   
       /:/  /  \::/__/      \:\  \  
      /:/  /    \:\__\       \:\  \ 
     /:/  /      \/__/        \:\__\
     \/__/                     \/__/

```

# Mil

A *small, concatenative* programming language. Implemented in C99.

Mainly influenced by GNU `dc` and Forth.

> ⚠️ Mil is incomplete, and can contain bugs and nasty stuff.
> You can go to the [TODO.txt](https://github.com/HoangTuan110/mil/blob/main/TODO.txt)
> for a list of completed and incomplete things.
> If you find a bug in Mil, [please open an issue](https://github.com/HoangTuan110/mil/issues).
> <br>
> Mil is also a primarily hobby project, so features that you
> expected from other small C programming languages are not here yet,
> like GC and C APIs. These may be implemented in the future.

[Getting started](#getting-started) •
[Overview](#overview) •
[Installation](#installation) •
[License](#license)

</div>

## Getting started

```
# A small taste of Mil
"Hello Mil!" .
50 40 + sa # a = (40 + 50)
30 20 - sb # b = (20 - 30)
la lb = .  # print(a == b)
```

See `examples/` for more examples about Mil.

## Overview
* Small — less than 300 sloc in a single .h/.c pair
* Compiles down to less than 40kb
* Supports numbers, strings and functions
<!-- * Simple mark and sweep GC -->
* Implemented in portable C99

## Installation

```sh
git clone https://github.com/HoangTuan110/mil
cd mil
chmod +x build.sh
./build.sh
```

## License

This program in under the MIT license. See LICENSE for more details.

</div>
