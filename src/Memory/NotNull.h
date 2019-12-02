///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_NOTNULL_H_
#define MEMORY_NOTNULL_H_

#include <memory>

#include "Common/Logging.h"

template <typename T>
class NotNull
{
public:
    NotNull(T t) : ptr_(t) { R_ASSERT(t != nullptr, "t cannot be nullptr"); }

    NotNull(const NotNull& other) = default;

    template <typename U,
              typename = std::enable_if<std::is_convertible_v<U, T>>>
    NotNull(const NotNull<U>& other) : ptr_(other.get()) {}

    auto get() const { return ptr_; }

    operator T() const { return get(); }
    auto operator->() const { return get(); }

    NotNull(std::nullptr_t) = delete;
    NotNull(int) = delete;

    auto operator++(int) -> NotNull = delete;
    auto operator--(int) -> NotNull = delete;

    auto operator++() -> NotNull& = delete;
    auto operator--() -> NotNull& = delete;

    auto operator+(size_t) -> NotNull& = delete;
    auto operator-(size_t) -> NotNull& = delete;

    auto operator=(std::nullptr_t) -> NotNull& = delete;
    auto operator=(int) -> NotNull& = delete;

    auto operator+=(size_t) -> NotNull& = delete;
    auto operator-=(size_t) -> NotNull& = delete;

private:
    T ptr_;
};

#endif
