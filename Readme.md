# RetDec lldb plugin
RetDec plugin for LLDB. RetDec is a machine-code decompiler based on LLVM.

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/ant4g0nist/ManuFuzzer/pulls)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/ant4g0nist/decompiler/blob/main/LICENSE)
[![Follow Twitter](https://img.shields.io/twitter/follow/ant4g0nist?style=social)](https://twitter.com/ant4g0nist)


<img src="imgs/Screenshot 2023-06-13 at 03.21.46.png" alt="Decompile command output screenshot" style="zoom:50%;" />

## Build

Packages should be preferably installed via [Homebrew](https://brew.sh).

* macOS >= 10.15
* Full Xcode installation ([including command-line tools](https://github.com/frida/frida/issues/338#issuecomment-426777849), see [#425](https://github.com/avast/retdec/issues/425) and [#433](https://github.com/avast/retdec/issues/433))
* [CMake](https://cmake.org/) (version >= 3.6)
* [Git](https://git-scm.com/)
* [OpenSSL](https://www.openssl.org/) (version >= 1.1.1)
* [Python](https://www.python.org/) (version >= 3.4)
* [autotools](https://en.wikipedia.org/wiki/GNU_Build_System) ([autoconf](https://www.gnu.org/software/autoconf/autoconf.html), [automake](https://www.gnu.org/software/automake/), and [libtool](https://www.gnu.org/software/libtool/))
* Optional: [Doxygen](http://www.stack.nl/~dimitri/doxygen/) and [Graphviz](http://www.graphviz.org/) for generating API documentation

```sh
brew install make
make
```

## Usage

```sh
plugin load build/src/decompiler.dylib
```

## Limitations
Currently only works with lldb built from source. Hopefully this will be fixed in the future.

## TODO
- [x] Decompile functions from target executable
- [ ] Add caching
- [ ] Enable spot decoding
- [ ] Add support for decoding any loaded module
- [ ] Syntax highlighter


## Thanks
- [retdec](https://github.com/avast/retdec)
