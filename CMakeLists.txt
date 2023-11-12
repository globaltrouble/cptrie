cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(cptrie VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Werror -Wall -Wextra -Wpedantic)

add_library(cptrie  ${CMAKE_CURRENT_SOURCE_DIR}/src/cptrie.cpp)

add_executable(build-cptrie ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(build-cptrie cptrie)