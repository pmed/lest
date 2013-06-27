// Copyright 2013 by Martin Moene
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LEST_LEST_H_INCLUDED
#define LEST_LEST_H_INCLUDED

#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <iterator>

#ifndef lest_NO_SHORT_ASSERTION_NAMES
# define EXPECT           lest_EXPECT
# define EXPECT_THROWS    lest_EXPECT_THROWS
# define EXPECT_THROWS_AS lest_EXPECT_TRHOWS_AS
#endif

#define lest_EXPECT( expr ) \
    try \
    { \
        if ( ! (expr) ) \
            throw lest::failure{ lest_LOCATION, #expr }; \
    } \
    catch( lest::failure const & ) \
    { \
        throw ; \
    } \
    catch( std::exception const & e ) \
    { \
        throw lest::unexpected{ lest_LOCATION, #expr, lest::with_message( e.what() ) }; \
    } \
    catch(...) \
    { \
        throw lest::unexpected{ lest_LOCATION, #expr, "of unknown type" }; \
    }

#define lest_EXPECT_THROWS( expr ) \
    for (;;) \
    { \
        try { lest::serum( expr ); } catch (...) { break; } \
        throw lest::expected{ lest_LOCATION, #expr }; \
    }

#define lest_EXPECT_TRHOWS_AS( expr, excpt ) \
    for (;;) \
    { \
        try { lest::serum( expr ); } catch ( excpt & ) { break; } catch (...) {} \
        throw lest::expected{ lest_LOCATION, #expr, lest::of_type( #excpt ) }; \
    }

#define lest_LOCATION lest::location{__FILE__, __LINE__}

namespace lest {

struct test
{
    const std::string name;
    const std::function<void()> behaviour;
};

struct test_group
{
    const std::string name;
    std::vector<test> tests;
};

struct location
{
    const std::string file;
    const int line;

    location( std::string file, int line )
    : file{ file }, line{ line } {}
};

struct comment
{
    const std::string text;

    comment( std::string text ) : text{ text } {}
    explicit operator bool() { return text.length() > 0; }
};

struct message : public std::runtime_error
{
    const std::string kind;
    const location where;
    const comment note;

    message( std::string kind, location where, std::string expr, std::string note = "" )
    : std::runtime_error{ expr }, kind{ kind }, where{ where }, note{ note } {}
};

struct failure : public message
{
    failure( location where, std::string expr )
    : message{ "failed", where, expr } {}
};

struct expected : public message
{
    expected( location where, std::string expr, std::string excpt = "" )
    : message{ "failed: didn't get exception", where, expr, excpt } {}
};

struct unexpected : public message
{
    unexpected( location where, std::string expr, std::string note )
    : message{ "failed: got unexpected exception", where, expr, note } {}
};

inline bool serum( bool verum ) { return verum; }

inline std::string with_message( std::string text )
{
    return "with message \"" + text + "\"";
}

inline std::string of_type( std::string text )
{
    return "of type " + text;
}

inline std::string pluralise( int n, std::string text )
{
    return n == 1 ? text : text + "s";
}

inline std::ostream & operator<<( std::ostream & os, comment note )
{
    return os << (note ? " " + note.text : "" );
}

inline std::ostream & operator<<( std::ostream & os, location where )
{
#ifndef __GNUG__
    return os << where.file << "(" << where.line << ")";
#else
    return os << where.file << ":" << where.line;
#endif
}

struct run_result
{
    unsigned failures;
    unsigned total;
};

inline void report( std::ostream & os, message const & e, std::string test )
{
    os << e.where << ": " << e.kind << e.note << ": " << test << ": " << e.what() << std::endl;
}

inline void summary( std::ostream & os, run_result const & result)
{
    if (result.failures > 0)
    {
        os << result.failures << " out of " << result.total << " " << pluralise(result.total, "test") << " failed." << std::endl;
    }
}

inline bool run( test const & t, std::ostream & os = std::cout )
{
    try
    {
        t.behaviour();
        return true;
    }
    catch (message const & e)
    {
        report(os, e, t.name);
        return false;
    }
}

template<typename ForwardIt>
run_result run(ForwardIt begin, ForwardIt end, std::ostream & os = std::cout)
{
    run_result result{ 0, 0 };

    for (ForwardIt it = begin; it != end; ++it)
    {
        result.failures += !run(*it, os);
        result.total += 1;
    }

    summary(os, result);

    return result;
}

template<std::size_t N>
run_result run(test const (&specification)[N], std::ostream & os = std::cout)
{
    return run(std::begin(specification), std::end(specification), os);
}

} // namespace lest

#endif // LEST_LEST_H_INCLUDED
