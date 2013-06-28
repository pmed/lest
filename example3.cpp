// Copyright 2013 by Martin Moene
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "lest.hpp"

using namespace std;

bool blow()
{
    throw std::runtime_error( "this is unexpected!");
    return true;
}

bool phew()
{
    throw 77;
    return true;
}

const lest::test_group specification[] =
{
    GROUP("Group 1",
    TEST("Numbers compare as usual", {
        EXPECT(1 < 2);
        EXPECT(blow()); EXPECT(2 > 1);
    }),
    TEST("Some tests simply succeed", {
        EXPECT(1 == 1);
        EXPECT(2 != 1);
    }),
    TEST("Some tests throw unknown exceptions", {
        EXPECT(1 < 2);
        EXPECT(phew());
        EXPECT(3 < 4);
    })),

    GROUP("Group 2",
    TEST("Expect exception (success)", {
        EXPECT_THROWS(phew());
        EXPECT_THROWS_AS(blow(), std::runtime_error);
    }),
    TEST("Expect exception (fail)", {
        EXPECT_THROWS(1 == 1);
    }),
        TEST("Expect given exception (fail)", {
            EXPECT_THROWS_AS(blow(), std::bad_alloc);
    }),
    TEST("Text compares lexically", {
        EXPECT(string("hello") < string("world"));
        EXPECT(string("hello") > string("world"));
        EXPECT(string("skipped") == string("assertion"));
    })),
};

int main()
{
    return lest::run( specification ).failures;
}

// cl -nologo -Wall -EHsc example2.cpp && example2
// g++ -Wall -Wextra -Weffc++ -std=c++11 -o example2.exe example2.cpp && example2

//--- Test group Group 1 ---
//d:\projects\lest\example3.cpp:37: failed: got unexpected exception with message "this is unexpected!": Numbers compare as usual: blow()
//d:\projects\lest\example3.cpp:37: failed: got unexpected exception of unknown type: Some tests throw unknown exceptions: phew()
//Group 1: 2 out of 3 tests failed.
//
//--- Test group Group 2 ---
//d:\projects\lest\example3.cpp:54: failed: didn't get exception: Expect exception (fail): 1 == 1
//d:\projects\lest\example3.cpp:54: failed: didn't get exception of type std::bad_alloc: Expect given exception (fail): blow()
//d:\projects\lest\example3.cpp:54: failed: Text compares lexically: string("hello") > string("world")
//Group 2: 3 out of 4 tests failed.
//
//Total: 5 out of 7 tests failed.
