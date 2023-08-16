cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

project(Direct2D)

add_executable(
    Direct2D
    ./src/main.cpp
    ./src/BaseWin.h
    ./src/MainWindow.h
    ./src/MainWindow.cpp
    ./src/Block.h
    ./src/Block.cpp
    ./src/Board.h
    ./src/Board.cpp
    ./src/ComPtr.h
)

target_compile_definitions(Direct2D PUBLIC UNICODE)

target_link_options(Direct2D PRIVATE "/SUBSYSTEM:WINDOWS")