#include <books.hpp>
#include <solver.hpp>

#include <exception>
#include <iomanip>
#include <sstream>

extern Solver juxSolver;
extern Solver maxSolver;

Solver &createSolver(const std::string &name) {
    if (name == "jux") {
        return juxSolver;
    }

    if (name == "max") {
        return maxSolver;
    }
    
    throw std::invalid_argument((
        std::ostringstream() << "Unrecognized solver: " << std::quoted(name)
    ).str());
}
