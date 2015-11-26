// Copyright (c) 2015 Vittorio Romeo
// License: AFL 3.0 | https://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#include <iostream>
#include <memory>
#include <vrm/core/static_if.hpp>

// "static if" and similar constructs would allow C++ developers to create
// compile-time branches that enable different parts of code depending on a
// compile-time condition, using familiar "imperative" syntax.

// proposals:

// N3613: "Static If" considered
// open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3613.pdf

// N4461: Static if resurrected
// open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4461.html

// Examples from N4461
// Motivating example 1:

/*
template <class T>
void f(T&& t)
{
    // handle one T
}
template <class T, class... Rest>
void f(T&& t, Rest&&... r)
{
    f(t);
    // handle the tail
    f(r...);
}

// With static if:
template <class T, class... Rest>
void f(T&& t, Rest&&... r)
{
    //  handle one T

    static_if (sizeof...(r))
    {
        // handle the tail
        f(r...);
    }
}
*/

/*
template <class T, class... Args>
enable_if_t<is_constructible_v<T, Args...>, unique_ptr<T>> make_unique(
    Args&&... args)
{
    return unique_ptr<T>(new T(forward<Args>(args)...));
}
template <class T, class... Args>
enable_if_t<!is_constructible_v<T, Args...>, unique_ptr<T>> make_unique(
    Args&&... args)
{
    return unique_ptr<T>(new T{forward<Args>(args)...});
}

// With static if:
template <class T, class... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    static_if(is_constructible_v<T, Args...>)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }
    else
    {
        return unique_ptr<T>(new T{forward<Args>(args)...});
    }
}
*/

// A working "static if" can actually be implemented in C++14, thanks to generic
// lambdas and SFINAE.

// My implementation was inspired by this article, by Baptiste Wicht:
// baptiste-wicht.com/posts/2015/07/simulate-static_if-with-c11c14.html

// And the comments on its reddit thread:
// reddit.com/r/cpp/comments/3d46ns/simulate_static_if_with_c11c14/

// TODO:

struct banana
{
    void eat()
    {
    }
};

struct peanuts
{
    void eat()
    {
    }
};

struct water
{
    void drink()
    {
    }
};

struct juice
{
    void drink()
    {
    }
};

template <typename T>
constexpr bool is_solid{false};

template <>
constexpr bool is_solid<banana>{true};

template <>
constexpr bool is_solid<peanuts>{true};

template <typename T>
constexpr bool is_liquid{false};

template <>
constexpr bool is_liquid<water>{true};

template <>
constexpr bool is_liquid<juice>{true};

template <typename T>
auto consume(T&& x)
{
    using namespace vrm::core;

    static_if(bool_<is_solid<T>>{})
        .then([](auto&& y)
            {
                y.eat();
                std::cout << "ate solid food\n";
            })
        .else_if(bool_<is_liquid<T>>{})
        .then([](auto&& y)
            {
                y.drink();
                std::cout << "drank liquid food\n";
            })
        .else_([](auto&&)
            {
                std::cout << "cannot consume\n";
            })(FWD(x));
}

int main()
{
    consume(banana{});
    consume(water{});
    consume(peanuts{});
    consume(juice{});
    consume(int{});
    consume(float{});

    return 0;
}