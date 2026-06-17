cmake_minimum_required(VERSION 3.20) # Ставим стабильный минимум, раз 4.0 поддерживается
project(Laba4 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(Laba4
        main.cpp
        Utils.h
        Ordinal.h
        Generators.h
        LazySequence.h
        Stream.h
        Algebra.h
        RollingWindow.h
)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(RunTests
        tests.cpp
)

# Разрешаем тестам видеть твои заголовочные файлы
target_include_directories(RunTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Линкуем с gtest
target_link_libraries(RunTests PRIVATE gtest)

include(GoogleTest)
gtest_discover_tests(RunTests)