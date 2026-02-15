FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    make \
    zlib1g-dev \
    lldb-18 \
    liblldb-18-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN mkdir -p build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && cmake --build . -j"$(nproc)"

# Smoke test: verify the plugin loads without error
RUN lldb-18 -o "plugin load /src/build/lldbghidra.so" -o "quit"

# Pre-compile the test binary (decompilation tests need ptrace, so run at container runtime)
RUN gcc -g -o /src/test/test_binary /src/test/docker_test.c
