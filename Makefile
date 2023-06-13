.DEFAULT_GOAL := build

build:
	brew install openssl autoconf automake libtool cmake
	# clang++ plugin.cpp -std=c++11 -framework LLDB -I ./headers/include -dynamiclib -o retdec.dylib
	mkdir -p build && cd build && OPENSSL_ROOT_DIR=`brew --prefix openssl` cmake ..
	cd build && make -j8

example:
	clang examples/sample.m -o examples/sample -framework Foundation
	~/llvm-16.0.4/bin/lldb -s examples/lldb_commands