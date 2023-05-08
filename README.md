[![Github CI Status](https://github.com/binji/binjnes/workflows/CI/badge.svg)](https://github.com/binji/binjnes)

# binjnes

A simple NES emulator.

## Features

* Cycle accurate, passes many timing tests (see below)
* Supports quite a few mappers
  * 0, 1, 2, 3, 4, 5, 7, 9, 10, 11, 19, 21, 23, 24, 25, 26, 28, 34, 66, 69, 71, 87, 206
  * Has built-in cart database
  * Supports additional audio channels for mapper 19, 24, 26
* Save/load battery backup
* Save/load emulator state to file
* Pause and step one frame
* **Rewind** and seek to specific cycle
* Convenient Python test harness using hashes to validate

## Cloning

Use a recursive clone, to include the submodules:

```
$ git clone --recursive https://github.com/binji/binjnes
```

If you've already cloned without initializing submodules, you can run this:

```
$ git submodule update --init
```

## Building

Requires [CMake](https://cmake.org) and
[sokol](https://github.com/floooh/sokol).

### Building (Linux/Mac)

If you run `make`, it will run CMake for you and put the output in the `bin/`
directory.

```
$ make
$ bin/binjnes foo.nes
```

You can also just use cmake directly:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Building WebAssembly

You can build binjnes as a WebAssembly module. You'll need an incoming build of
emscripten. See https://github.com/kripken/emscripten/wiki/WebAssembly and
http://kripken.github.io/emscripten-site/docs/building_from_source/index.html#installing-from-source.

Put a symlink to Emscripten in the `emscripten` directory, then run make.

```
$ ln -s ${PATH_TO_EMSCRIPTEN} emscripten
$ make wasm
```
Or set Makefile variables via command line:
```
$ make wasm EMSCRIPTEN_CMAKE="/path/to/Emscripten.cmake"
```

### Changing the Build Configuration

If you change the build config (e.g. update the submodules), you may need to run CMake again.
The simplest way to do this is to remove the `out/` directory.

```
$ rm -rf out/
$ make
```

## Running

```
$ bin/binjnes <filename>
```

Keys:

| Action | Key |
| --- | --- |
| DPAD-UP | <kbd>↑</kbd> |
| DPAD-DOWN | <kbd>↓</kbd> |
| DPAD-LEFT | <kbd>←</kbd> |
| DPAD-RIGHT | <kbd>→</kbd> |
| B | <kbd>Z</kbd> |
| A | <kbd>X</kbd> |
| START | <kbd>Enter</kbd> |
| SELECT | <kbd>Tab</kbd> |
| Save state | <kbd>F6</kbd> |
| Load state | <kbd>F9</kbd> |
| Rewind | <kbd>Backspace</kbd> |
| Pause | <kbd>Space</kbd> |
| Step one frame | <kbd>N</kbd> |

## Running tests

`scripts/tester.py` will only run the tests that match a filter passed on the
command line. Some examples:

```
# Run all tests
$ scripts/tester.py

# Run all apu tests
$ scripts/tester.py apu
```

## Test status

[See test results](test_results.md)
