#pragma 

#include <vector>

struct Library {
    unsigned int id = 0;
    unsigned int signUpTime;
    unsigned int throughput;
    unsigned int totalPoints;
    std::vector<unsigned int> books;
};

struct Problem {
    unsigned int bookCount;
    unsigned int libraryCount;
    unsigned int dayCount;
    std::vector<unsigned int> bookScores;
    std::vector<Library> libraries;
};

struct Subscription {
    unsigned int libraryId;
    std::vector<unsigned int> bookIds;
};

struct Solution {
    std::vector<Subscription> subscriptions;
};

Solution solve(const Problem &problem);