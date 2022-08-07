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

[Getting started](#getting-started) •
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
