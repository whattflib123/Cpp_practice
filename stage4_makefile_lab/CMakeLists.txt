cmake_minimum_required(VERSION 3.10) # 
project(phase_4)

add_executable(app main.cpp src/math_utils.cpp)

target_include_directories(app PRIVATE include) 