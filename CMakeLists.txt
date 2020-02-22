cmake_minimum_required(VERSION 3.15)

project(hashcode-2020-books)

list(APPEND SOLVERS
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/solvers/jux1.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/solvers/jux2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/solvers/jux3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/solvers/max.cpp"
)

set(EXPORT_SOLVER_CPP "${CMAKE_BINARY_DIR}/solver.cpp")
set(EXPORT_SOLVER_CPP_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/cpp/solver.cmake")

add_custom_command(
    OUTPUT "${EXPORT_SOLVER_CPP}"
    COMMAND ${CMAKE_COMMAND}
        -D SOLVERS="${SOLVERS}"
        -D EXPORT_SOLVER_CPP="${EXPORT_SOLVER_CPP}"
        -P "${EXPORT_SOLVER_CPP_GENERATOR}"
    DEPENDS
        "${EXPORT_SOLVER_CPP_GENERATOR}"
)

add_executable(${PROJECT_NAME}
    cpp/books.hpp
    cpp/solver.hpp
    cpp/problem.cpp
    cpp/solution.cpp
    cpp/main.cpp
    ${EXPORT_SOLVER_CPP}
    ${SOLVERS}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/cpp"
        "${CMAKE_BINARY_DIR}"
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
