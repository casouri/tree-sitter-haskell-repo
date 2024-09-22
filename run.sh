#!/bin/bash

rm -rf tree-sitter tree-sitter-module

git clone https://github.com/tree-sitter/tree-sitter.git --depth=1
git clone https://github.com/casouri/tree-sitter-module.git --depth=1

cd tree-sitter
make
cp libtree-sitter* ..
cd ../tree-sitter-module
./build.sh haskell
cp dist/* ..
cd ..

gcc recipe.c -o recipe -ltree-sitter -ltree-sitter-haskell -L. -I./tree-sitter/lib/include
./recipe
