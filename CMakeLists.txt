cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(AISnake)


add_executable(AISnake main.cpp Snake.hpp Snake.cpp)

target_link_libraries (AISnake glut GLU GL m pthread)
