# tiny-calc

**TODO:** Project description.

## Project Structure

- `src/`: Source code of the project lives here
- `tests/`: Snapshot test files live here
- `build/`: Temporary folder that contains build artifacts
- `COMPILE.txt`: Command for compiling a release build of the project
- `TEST.txt`: Documents all test cases and their user-perceived output
- `build.py`: Compiles the project for development and generates `COMPILE.txt`
- `test.py`: Validates snapshot tests and generates `TEST.txt`
- `.clang-format`: Configuration file for `clang-format`

## Building

Make sure that you have a modern version of `gcc` installed before building.

To build the project simply execute the command found in `COMPILE.txt`.

## Development

### Setup

Make sure that you have these tools installed before doing a development build or running the tests:

- `just` https://github.com/casey/just
- `gcc` https://gcc.gnu.org
- `clang-format` https://clang.llvm.org
- `sccache` https://github.com/mozilla/sccache
- `git` https://git-scm.com
- `python` https://www.python.org

### Usage

- `just build` or `just b` are used to build the project
- `just run` or `just r` are used to build and run the project
- `just test` or `just t` are used to run all tests
- `just format` to apply fromatting to all C++ files located in `src/`

### Tipps

Prefix the compile command with `SCCACHE_RECACHE=1` when changing `build.py` to force `scache` to use the new configuration and recompile everything.

Example:

```bash
# changed compiler flags in build.py
$ SCCACHE_RECACHE=1 just run
# everything gets recompiled and then tiny-calc gets run
```
