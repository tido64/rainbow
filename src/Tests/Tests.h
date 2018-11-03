// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <functional>

namespace rainbow
{
    template <typename T>
    using out = std::reference_wrapper<T>;

    auto run_tests(int argc, char* argv[]) -> int;

    bool should_run_tests(out<int> argc, out<char**> argv);
}  // namespace rainbow
