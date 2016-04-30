// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#define DEFINE_NOT_FN(name, fn, in_type)                                       \
    rainbow::Not<decltype(&fn), in_type> name{fn}

namespace rainbow
{
    template <typename P, typename T>
    class Not
    {
    public:
        Not(P predicate) : pred_(predicate) {}

        bool operator()(T a) const { return !pred_(a); }
        bool operator()(T a, T b) const { return !pred_(a, b); }

    private:
        P pred_;
    };
}
