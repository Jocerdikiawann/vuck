@echo off

IF EXIST build (
    echo Directory build exists.
) ELSE (
    echo Directory build does not exist. Creating now...
    mkdir build
)

cd build
rd /s /q .
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
mingw32-make
vuck.exe
cd ..
