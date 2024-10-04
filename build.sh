#!bin/sh

set -xe

# cd build
# cmake ..
# make
# main
gcc -O0 -g ./*.c -o ./main -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -lvulkan
main
