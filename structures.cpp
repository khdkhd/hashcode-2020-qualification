#include <vector>

struct Book
{
        unsigned score;
};

struct Library
{
        std::vector<Book> books;

        unsigned id;
        unsigned signUpTime;
        unsigned output;
        unsigned totalPoints;
};

struct Subscription
{
        unsigned id;
        std::vector<unsigned> books;
};

struct Solution
{
        std::vector<Subscription> books;
};
