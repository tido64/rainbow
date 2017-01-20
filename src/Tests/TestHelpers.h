// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <type_traits>
#include <utility>

namespace rainbow { namespace test
{
    namespace detail
    {
        template <typename F>
        class Not
        {
        public:
            explicit Not(F&& predicate) noexcept
                : predicate_(std::move(predicate))
            {
            }

            template <typename... Args>
            bool operator()(Args&&... args) const
            {
                return !predicate_(std::forward<Args>(args)...);
            }

        private:
            F predicate_;
        };
    }

    template <typename F>
    auto not_fn(F&& predicate)
    {
        return detail::Not<std::decay_t<F>>{std::forward<F>(predicate)};
    }
}}  // namespace rainbow::test

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting
    {
    };
}
