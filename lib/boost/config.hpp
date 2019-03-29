#ifndef BOOST_CONFIG_HPP
#define BOOST_CONFIG_HPP

//  ----- detail -----

#define BOOST_CONSTEXPR_OR_CONST  constexpr
#define BOOST_NOEXCEPT  noexcept
#define BOOST_STATIC_CONSTEXPR  static BOOST_CONSTEXPR_OR_CONST

//  ----- system -----

# if defined(_WIN32) || defined(__CYGWIN__) // Windows default, including MinGW and Cygwin
#   define BOOST_WINDOWS_API 1
# else
#   define BOOST_POSIX_API 1
# endif

//  ----- utility -----

#include <type_traits>

namespace boost
{
    using std::decay;
    using std::is_array;
    using std::is_integral;
}

#endif  // BOOST_CONFIG_HPP
