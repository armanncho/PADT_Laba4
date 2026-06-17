cmake_minimum_required(VERSION 4.0)
project(Laba4)

set(CMAKE_CXX_STANDARD 20)

add_executable(Laba4 main.cpp
        Utils.h
        "Ordinal.h"
        Generators.h
        LazySequence.h
        Stream.h
        Algebra.h
        RollingWindow.h)
