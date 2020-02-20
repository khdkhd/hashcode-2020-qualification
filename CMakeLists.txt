cmake_minimum_required(VERSION 3.15)

project(hashcode-2020-books)

add_executable(${PROJECT_NAME}
    cpp/books.hpp
    cpp/solver.cpp
    cpp/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)