cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(cptrie VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Werror -Wall -Wextra -Wpedantic)

add_library(cptrie  ${CMAKE_CURRENT_SOURCE_DIR}/src/cptrie.cpp)
target_include_directories(cptrie PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(build-cptrie 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/build.cpp
)
target_link_libraries(build-cptrie cptrie)