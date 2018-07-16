// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/TypeInfo.h"

using rainbow::type_id;
using rainbow::type_id_t;

class ForwardedClassDecl;

TEST(TypeInfoTest, ConvertsLValuesAndRemovesCVQualifiers)
{
    ASSERT_EQ(type_id<int&>(), type_id<int>());
    ASSERT_EQ(type_id<int&&>(), type_id<int>());
    ASSERT_EQ(type_id<const int>(), type_id<int>());
    ASSERT_EQ(type_id<const int&>(), type_id<int>());
    ASSERT_EQ(type_id<const int&&>(), type_id<int>());
    ASSERT_EQ(type_id<volatile int>(), type_id<int>());
    ASSERT_EQ(type_id<volatile int&>(), type_id<int>());
    ASSERT_EQ(type_id<volatile int&&>(), type_id<int>());
    ASSERT_EQ(type_id<const volatile int>(), type_id<int>());
    ASSERT_EQ(type_id<const volatile int&>(), type_id<int>());
    ASSERT_EQ(type_id<const volatile int&&>(), type_id<int>());

    ASSERT_NE(type_id<char>(), type_id<char*>());
    ASSERT_NE(type_id<const char*>(), type_id<char*>());
    ASSERT_NE(type_id<volatile char*>(), type_id<char*>());
}

TEST(TypeInfoTest, IsAssignable)
{
    auto int_type_id = type_id<int>();
    auto type = int_type_id;

    ASSERT_EQ(type, type_id<int>());

    type = type_id<char>();

    ASSERT_EQ(type, type_id<char>());
    ASSERT_EQ(int_type_id, type_id<int>());
}

TEST(TypeInfoTest, ReturnsUniqueTypeIds)
{
    auto const_void_ptr_id = type_id<const void*>();
    auto const_void_ptr_const_id = type_id<const void* const>();
    auto int_type_id = type_id<int>();
    auto float_type_id = type_id<float>();
    auto forwarded_class_type_id = type_id<ForwardedClassDecl>();
    auto forwarded_class_ptr_type_id = type_id<ForwardedClassDecl*>();
    auto nullptr_type_id = type_id<std::nullptr_t>();
    auto type_id_t_type_id = type_id<type_id_t>();
    auto uint_type_id = type_id<unsigned int>();

    ASSERT_EQ(type_id<const void*>(), const_void_ptr_id);
    ASSERT_EQ(type_id<const void*>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<const void*>(), int_type_id);
    ASSERT_NE(type_id<const void*>(), float_type_id);
    ASSERT_NE(type_id<const void*>(), forwarded_class_type_id);
    ASSERT_NE(type_id<const void*>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<const void*>(), nullptr_type_id);
    ASSERT_NE(type_id<const void*>(), type_id_t_type_id);
    ASSERT_NE(type_id<const void*>(), uint_type_id);

    ASSERT_NE(type_id<int>(), const_void_ptr_id);
    ASSERT_NE(type_id<int>(), const_void_ptr_const_id);
    ASSERT_EQ(type_id<int>(), int_type_id);
    ASSERT_NE(type_id<int>(), float_type_id);
    ASSERT_NE(type_id<int>(), forwarded_class_type_id);
    ASSERT_NE(type_id<int>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<int>(), nullptr_type_id);
    ASSERT_NE(type_id<int>(), type_id_t_type_id);
    ASSERT_NE(type_id<int>(), uint_type_id);

    ASSERT_NE(type_id<float>(), const_void_ptr_id);
    ASSERT_NE(type_id<float>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<float>(), int_type_id);
    ASSERT_EQ(type_id<float>(), float_type_id);
    ASSERT_NE(type_id<float>(), forwarded_class_type_id);
    ASSERT_NE(type_id<float>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<float>(), nullptr_type_id);
    ASSERT_NE(type_id<float>(), type_id_t_type_id);
    ASSERT_NE(type_id<float>(), uint_type_id);

    ASSERT_NE(type_id<ForwardedClassDecl>(), const_void_ptr_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), int_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), float_type_id);
    ASSERT_EQ(type_id<ForwardedClassDecl>(), forwarded_class_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), nullptr_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), type_id_t_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl>(), uint_type_id);

    ASSERT_NE(type_id<ForwardedClassDecl*>(), const_void_ptr_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), int_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), float_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), forwarded_class_type_id);
    ASSERT_EQ(type_id<ForwardedClassDecl*>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), nullptr_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), type_id_t_type_id);
    ASSERT_NE(type_id<ForwardedClassDecl*>(), uint_type_id);

    ASSERT_NE(type_id<std::nullptr_t>(), const_void_ptr_id);
    ASSERT_NE(type_id<std::nullptr_t>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<std::nullptr_t>(), int_type_id);
    ASSERT_EQ(type_id<std::nullptr_t>(), nullptr_type_id);
    ASSERT_NE(type_id<std::nullptr_t>(), float_type_id);
    ASSERT_NE(type_id<std::nullptr_t>(), forwarded_class_type_id);
    ASSERT_NE(type_id<std::nullptr_t>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<std::nullptr_t>(), type_id_t_type_id);
    ASSERT_NE(type_id<std::nullptr_t>(), uint_type_id);

    ASSERT_NE(type_id<type_id_t>(), const_void_ptr_id);
    ASSERT_NE(type_id<type_id_t>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<type_id_t>(), int_type_id);
    ASSERT_NE(type_id<type_id_t>(), float_type_id);
    ASSERT_NE(type_id<type_id_t>(), forwarded_class_type_id);
    ASSERT_NE(type_id<type_id_t>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<type_id_t>(), nullptr_type_id);
    ASSERT_EQ(type_id<type_id_t>(), type_id_t_type_id);
    ASSERT_NE(type_id<type_id_t>(), uint_type_id);

    ASSERT_NE(type_id<unsigned int>(), const_void_ptr_id);
    ASSERT_NE(type_id<unsigned int>(), const_void_ptr_const_id);
    ASSERT_NE(type_id<unsigned int>(), int_type_id);
    ASSERT_NE(type_id<unsigned int>(), float_type_id);
    ASSERT_NE(type_id<unsigned int>(), forwarded_class_type_id);
    ASSERT_NE(type_id<unsigned int>(), forwarded_class_ptr_type_id);
    ASSERT_NE(type_id<unsigned int>(), nullptr_type_id);
    ASSERT_NE(type_id<unsigned int>(), type_id_t_type_id);
    ASSERT_EQ(type_id<unsigned int>(), uint_type_id);
}
