#include <books.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>
#include <random>

namespace {
unsigned int
remainingDays(Problem &problem, unsigned int days) {
    if (days >= problem.dayCount) {
        return 0;
    }
    return problem.dayCount - days;
}

std::intmax_t
getMaxIteration(const Options &options) {
    const auto entry = options.find("max-iteration");
    return entry != options.end()
        ? std::get<std::intmax_t>(entry->second)
        : 1000;
}

struct count_iterator {
    using difference_type = std::intmax_t;
    using value_type = std::uintmax_t;
    using pointer = void;
    using reference = value_type;
    using iterator_category = std::input_iterator_tag;

    explicit count_iterator(value_type v = 0)
        : value{v} {
    }

    count_iterator &operator++() {
        value = value + 1;
        return *this;
    }

    count_iterator operator++(int) {
        count_iterator ret = *this;
        ++(*this);
        return ret;
    }

    bool operator==(const count_iterator &rhs) const {
        return value == rhs.value;
    }

    bool operator!=(const count_iterator &rhs) const {
        return !(*this == rhs);
    }

    reference operator*() const {
        return value;
    }

    value_type value;
};
}

Solver jux4Solver([](Problem &problem, const Options &options) {
    // Sort libraries books by their score and remove duplicates
    std::cerr << "sort libraries books" << std::endl;
    for (auto &library: problem.libraries) {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&problem](auto book_id1, auto book_id2) {
                return problem.bookScores[book_id1] < problem.bookScores[book_id2];
            }
        );
    }
    std::cerr << "sort libraries books - done!" << std::endl;

    // Compute solution
    std::cerr << "compute solution" << std::endl;

    std::random_device rd;
    std::mt19937 random_generator(rd());

    const auto max_iteration = getMaxIteration(options);
    std::cerr << "max_iteration: " << max_iteration << std::endl;

    auto solution = std::reduce(
        count_iterator(),
        count_iterator(max_iteration),
        Solution(),
        [&random_generator, &problem](auto best_solution, auto iteration) {
            std::bitset<1000000> bookState; // 0: already sent, 1: available
            Solution solution;

            bookState.set();

            std::shuffle(
                problem.libraries.begin(),
                problem.libraries.end(),
                random_generator
            );

            auto sign_up_offset = 0u;
            for (const auto &library: problem.libraries) {
                sign_up_offset += library.signUpTime;
                auto bookCount = remainingDays(problem, sign_up_offset)*library.throughput;

                Subscription subscription{library.id};

                if (bookState.any()) {
                    for (auto book_id: library.books) {
                        if (bookCount == 0) {
                            break;
                        }
                        if (bookState[book_id]) {
                            bookState[book_id] = false;
                            bookCount = bookCount - 1;
                            subscription.bookIds.emplace_back(book_id);
                        }
                    }
                    if (subscription.bookIds.size() > 0) {
                        solution.subscriptions.emplace_back(subscription);
                    }
                }
            }

            const auto solution_score = score(problem, solution);
            const auto best_solution_score = score(problem, best_solution);

            // std::cerr << "          iteration: " << iteration << std::endl;
            // std::cerr << "     solution_score: " << solution_score << std::endl;
            // std::cerr << "best_solution_score: " << best_solution_score << std::endl;
            // std::cerr << "--" << std::endl;

            return  solution_score > best_solution_score
                ? solution
                : best_solution;
        }
    );
    std::cerr << "compute solution - done!" << std::endl;

    return solution;
});

