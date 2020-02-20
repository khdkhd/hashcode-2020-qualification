cmake_minimum_required(VERSION 3.15)

project(hashcode-2020-books)

add_executable(${PROJECT_NAME} cpp/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)