cmake_minimum_required(VERSION 3.22)

project(TicketSystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2")
include_directories(include)

add_executable(code main.cpp
    src/user.cpp src/train.cpp src/order.cpp)