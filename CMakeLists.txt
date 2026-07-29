set(CMAKE_BUILD_TYPE "Debug")

cmake_minimum_required(VERSION 3.21)
project(sdl3-android-example-c)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(app SHARED)

find_package(SDL3 REQUIRED CONFIG)

target_sources(app PRIVATE src/main.c)

target_link_libraries(app PRIVATE SDL3::SDL3)
