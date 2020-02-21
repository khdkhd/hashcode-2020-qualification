separate_arguments(SOLVERS)

file(WRITE "${EXPORT_SOLVER_CPP}" [=[
// 🤖 Do not edit. This is a generated file. 🤖

#include <books.hpp>
#include <solver.hpp>

#include <exception>
#include <iomanip>
#include <sstream>

]=])

foreach(SOLVER ${SOLVERS})
    get_filename_component(SOLVER_NAME "${SOLVER}" NAME_WE)
    file(APPEND "${EXPORT_SOLVER_CPP}" "extern Solver ${SOLVER_NAME}Solver;\n")
endforeach()

file(APPEND "${EXPORT_SOLVER_CPP}" [=[

Solver &
createSolver(const std::string &name) {
]=])


foreach(SOLVER ${SOLVERS})
    get_filename_component(SOLVER_NAME "${SOLVER}" NAME_WE)
    file(APPEND "${EXPORT_SOLVER_CPP}" "    if (name == \"${SOLVER_NAME}\") return ${SOLVER_NAME}Solver;\n")
endforeach()

file(APPEND "${EXPORT_SOLVER_CPP}" [=[
    throw std::invalid_argument((
        std::ostringstream() << "Unrecognized solver: " << std::quoted(name)
    ).str());
}
]=])