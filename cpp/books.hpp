#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <variant>
#include <vector>

constexpr unsigned int MaxBookCount = 100000;

struct Library {
    using Books = std::vector<unsigned int>;
    using BookIterator = std::vector<unsigned int>::iterator;
    using BookConstIterator = std::vector<unsigned int>::const_iterator;

    unsigned int id{0};
    unsigned int signUpTime{0};
    unsigned int throughput{0};
    unsigned int bookCount{0};
    Books books;
};

struct Problem {
    unsigned int bookCount{0};
    unsigned int libraryCount{0};
    unsigned int dayCount{0};
    std::vector<unsigned int> bookScores;
    std::vector<Library> libraries;
};

std::istream &operator>>(std::istream &, Problem &);
std::ostream &operator<<(std::ostream &, const Problem &);

struct Subscription {
    unsigned int libraryId{0};
    std::vector<unsigned int> bookIds;
};

struct Solution {
    unsigned int score{0};
    std::vector<Subscription> subscriptions;
};

std::ostream &operator<<(std::ostream &, const Solution &);

unsigned int score(const Problem &, const Solution &);

using OptionKey = std::string;
using OptionValue = std::variant<intmax_t, double, std::string>;
using Options = std::map<OptionKey, OptionValue>;

using Solver = std::function<Solution(Problem &, const Options &)>;
