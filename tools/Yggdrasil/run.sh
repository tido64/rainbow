#!/bin/sh
source=$(cd -P "$(dirname $0)" && pwd)
clang++ -std=c++1z -stdlib=libc++ -g -O0 -Wall -I /usr/local/opt/llvm/include -I "${source}" -L /usr/local/opt/llvm/lib -lclang "${source}/main.cpp" -o yggdrasil || exit 1
./yggdrasil
