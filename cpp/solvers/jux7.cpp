#include <books.hpp>

#include <cstdlib>
#include <algorithm>
#include <bitset>
#include <deque>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <unordered_set>
#include <type_traits>
#include <utility>

namespace {

template<typename T = std::uintmax_t>
struct RangeIterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using reference = value_type;
    using difference_type = void;
    using pointer = void;

    explicit RangeIterator(value_type v = 0)
        : value{v} {
    }

    RangeIterator &operator++() {
        value = value + 1;
        return *this;
    }

    RangeIterator operator++(int) {
        RangeIterator ret = *this;
        ++(*this);
        return ret;
    }

    bool operator==(const RangeIterator &rhs) const {
        return value == rhs.value;
    }

    bool operator!=(const RangeIterator &rhs) const {
        return !(*this == rhs);
    }

    reference operator*() const {
        return value;
    }

    value_type value;
};

template<typename T = std::intmax_t>
struct Range {
    using value_type = T;
    using iterator = RangeIterator<value_type>;

    Range(value_type from, value_type to)
        : from_{from}
        , to_{to} {
    }

    Range(value_type to)
        : from_{0}
        , to_{to} {
    }

    iterator begin() const {
        return iterator(from_);
    }

    iterator end() const {
        return iterator(to_);
    }

private:
    value_type from_;
    value_type to_;
};

struct PendingLibrary {
    unsigned int id;
    unsigned int signUpDay;
};

struct LibrarySubscription {
    unsigned int id;
    unsigned int currentBookIndex{0};
    std::vector<unsigned int> bookIds;

    bool isOver(const Problem &problem) const {
        return currentBookIndex >= problem.libraries[id].books.size();
    }

    unsigned int score(
        const Problem &problem,
        const std::bitset<MaxBookCount> &bookState
    ) const {
        if (isOver(problem)) return 0;

        const auto &library = problem.libraries[id];
        const auto last = library.books.end();
        auto it = library.books.begin() + currentBookIndex;

        return std::reduce(
            RangeIterator(0u),
            RangeIterator(library.throughput),
            0u,
            [&it, &last, &bookState](auto score, auto) {
                it = std::find_if(it, last, [&bookState](auto book_id) {
                    return bookState[book_id];
                });
                return score + (it != last ? *it : 0);
            }
        );
    }

    void sendBook(
        const Problem &problem,
        std::bitset<MaxBookCount> &bookState
    ) {
        const auto &library = problem.libraries[id];
        const auto last = library.books.end();
        auto it = library.books.begin() + currentBookIndex;

        if (it < last) {
            for (auto _: Range(library.throughput)) {
                it = std::find_if(it, last, [&bookState](auto book_id) {
                    return bookState[book_id];
                });

                if (it == last) break; // no more books

                bookState[*it] = false;
                bookIds.emplace_back(*it);
            }
            currentBookIndex = std::distance(library.books.begin(), it);
        }
    }
};
}

std::bitset<MaxBookCount>
libraryBookMask(const Library &library) {
    return std::reduce(
        library.books.begin(), library.books.end(),
        std::bitset<MaxBookCount>(),
        [](auto &&books, auto book_id) {
            return std::move(books.set(book_id));
        }
    );
}

void
sortLibraries(Problem &problem) {
    auto first = problem.libraries.begin(), last = problem.libraries.end();

    std::vector<std::bitset<MaxBookCount>> masks;
    std::transform(first, last, std::back_inserter(masks), libraryBookMask);

    std::vector<unsigned int> library_bitsets;
    std::transform(
        first, last,
        std::back_inserter(library_bitsets),
        [first, last, &problem, &masks](const auto &current_library) {
            std::cerr << "scan library " << current_library.id << "/" << std::distance(first, last) << std::endl;
            auto mask = std::reduce(
                first, last,
                masks[current_library.id],
                [&current_library, &masks](auto &&mask, const auto &library) {
                    if (library.id == current_library.id) {
                        return std::move(mask);
                    }
                    return mask & ~masks[library.id];
                }
            );
            return mask.count();
        }
    );

    std::sort(first, last, [&library_bitsets](const auto &lhs, const auto &rhs) {
        if (library_bitsets[lhs.id] == library_bitsets[rhs.id]) {
            return lhs.signUpTime < rhs.signUpTime;
        }
        return library_bitsets[lhs.id] > library_bitsets[rhs.id];
    });
}

Solver jux7Solver([](Problem &problem, const Options &options) {
    std::bitset<MaxBookCount> bookState; // 0: already sent, 1: available

    bookState.set();

    sortLibraries(problem);

    // Sort libraries books by their score and remove duplicates
    // std::cerr << "sort libraries books" << std::endl;
    for (auto &library: problem.libraries) {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&problem](auto book_id1, auto book_id2) {
                return problem.bookScores[book_id1] < problem.bookScores[book_id2];
            }
        );
    }
    // std::cerr << "sort libraries books - done!" << std::endl;

    // Sort libraries by their sign up delay
    // std::cerr << "sort libraries" << std::endl;
    // Compute library score
    // std::cerr << "compute libraries score" << std::endl;
    // std::vector<unsigned int> library_scores;
    // std::transform(
    //     problem.libraries.begin(),
    //     problem.libraries.end(),
    //     std::back_inserter(library_scores),
    //     [&problem](const auto &library) {
    //         const auto bookCount = (problem.dayCount - library.signUpTime)*library.throughput;
    //         return std::accumulate(
    //             library.books.begin(),
    //             library.books.begin() + std::min<std::size_t>(library.books.size(), bookCount),
    //             0u,
    //             [&problem](auto acc, auto book_id) {
    //                 return acc + problem.bookScores[book_id];
    //             }
    //         );
    //     }
    // );
    // std::cerr << "compute libraries score" << std::endl;

    // Sort library score
    // const auto average_score = std::accumulate(library_scores.begin(), library_scores.end(), 0.)/library_scores.size();


    // std::sort(
    //     problem.libraries.begin(),
    //     problem.libraries.end(),
    //     [average_score, &library_scores](const auto &library1, const auto &library2) {
    //         const auto lib1_score = library_scores[library1.id];
    //         const auto lib2_score = library_scores[library2.id];

    //         if (((lib1_score >= average_score) && (lib2_score >= average_score))
    //             || ((lib1_score < average_score) && (lib2_score < average_score))) {
    //             return library1.signUpTime == library2.signUpTime
    //                 ? lib1_score > lib2_score
    //                 : library1.signUpTime < library2.signUpTime;
    //         }

    //         if (lib1_score > average_score) {
    //             return true;
    //         }

    //         return false;
    //     }
    // );

    // std::cerr << "sort libraries - done" << std::endl;

    // std::cerr << "Prepare pending subscriptions queue" << std::endl;
    std::deque<PendingLibrary> pending_libraries;
    auto signUpDay = 0u;
    for (const auto &library : problem.libraries) {
        signUpDay += library.signUpTime;
        pending_libraries.push_back(PendingLibrary{
            library.id,
            signUpDay
        });
    }
    // std::cerr << "Prepare pending subscriptions queue - done" << std::endl;

    std::vector<LibrarySubscription> subscribed_libraries;

    for (const auto day : Range(problem.dayCount)) {
        // Check if there is a sign up process that end today
        if (pending_libraries.front().signUpDay == day) {
            const auto &library_id = pending_libraries.front().id;
            subscribed_libraries.emplace_back(LibrarySubscription{library_id});
            pending_libraries.pop_front();
        }

        std::list<std::reference_wrapper<LibrarySubscription>> active_subscribed_libraries;
        for (auto &subscribed_library: subscribed_libraries) {
            if (!subscribed_library.isOver(problem)) {
                active_subscribed_libraries.emplace_back(std::ref(subscribed_library));
            }
        }

        // std::cerr << "day " << day << "/" << problem.dayCount << " - " << active_subscribed_libraries.size() << " subscribed libraries" << std::endl;

        while (! active_subscribed_libraries.empty()) {
            auto it = std::max_element(
                std::begin(active_subscribed_libraries),
                std::end(active_subscribed_libraries),
                [&problem, &bookState](const auto &lhs, const auto &rhs) {
                    return lhs.get().score(problem, bookState) > rhs.get().score(problem, bookState);
                }
            );

            it->get().sendBook(problem, bookState);

            active_subscribed_libraries.erase(it);
        }
    }

    Solution solution;

    for (auto &subscribed_library: subscribed_libraries) {
        Subscription subscription{subscribed_library.id};
        std::swap(subscription.bookIds, subscribed_library.bookIds);
        solution.subscriptions.emplace_back(std::move(subscription));
    }

    return solution;
});

