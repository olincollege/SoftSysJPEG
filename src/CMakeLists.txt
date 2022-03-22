cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE Debug)
project(JPEG)
include_directories(../include)

add_executable(bmp_to_jpg bmp_to_jpg.c cbmp.c read_bmp_to_array.c dct.c)
target_link_libraries(bmp_to_jpg gsl m)