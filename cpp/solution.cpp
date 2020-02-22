#include <books.hpp>

#include <istream>
#include <ostream>
#include <numeric>

std::ostream &
operator<<(std::ostream &out, const Subscription &subscription) {
    out << subscription.libraryId << " " << subscription.bookIds.size() << '\n';
    std::copy(
        subscription.bookIds.begin(),
        subscription.bookIds.end(),
        std::ostream_iterator<unsigned int>(out, " ")
    );
    return out;
}

std::ostream &
operator<<(std::ostream &out, const Solution &solution) {
    out << solution.subscriptions.size() << '\n';
    std::copy(
        solution.subscriptions.begin(),
        solution.subscriptions.end(),
        std::ostream_iterator<Subscription>(out, "\n")
    );
    return out;
}

auto
subscriptionScore(const Problem &problem) {
    return [&problem](unsigned int score, const Subscription &subscription) {
        return std::reduce(
            subscription.bookIds.begin(),
            subscription.bookIds.end(),
            score,
            [&problem](auto score, auto book_id) {
                return score + problem.bookScores[book_id];
            }
        );
    };
}

unsigned int
score(const Problem &problem, const Solution &solution) {
    return std::reduce(
        solution.subscriptions.begin(),
        solution.subscriptions.end(),
        0u,
        subscriptionScore(problem)
    );
}
