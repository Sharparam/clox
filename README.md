# clox
A C implementation of the Lox language.

The implementation follows part III of [the book][book] by [munificent][].

## Usage

Launch it via `./build/clox` from the root of the repo after building it. Passing no arguments will start the REPL.

Pass a file as an argument to execute the file: `./build/clox myfile.lox`

For a description of some options available, run it with the `-h` or `--help` option.

## Building

Generate build files with [CMake][cmake] and then build as you're used to.

If you're on Linux with Ninja available:

```
$ mkdir build && cd build
$ cmake -G "Ninja" ..
$ ninja
```

## License

Copyright (c) 2018 by Adam Hellberg.

This project is licensed under the [MIT License][mit], following the [main repo][main] licensing the code part under MIT.

See the file `LICENSE` for more information.

[book]: http://www.craftinginterpreters.com/
[munificent]: https://github.com/munificent
[mit]: https://opensource.org/licenses/MIT
[main]: https://github.com/munificent/craftinginterpreters
[cmake]: https://cmake.org/
