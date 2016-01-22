// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/TypeInfo.h"

using rainbow::type_id;
using rainbow::type_id_t;

class ForwardedClassDecl;

TEST(TypeInfoTest, ConvertsLValuesAndRemovesCVQualifiers)
{
    ASSERT_EQ(type_id<int>(), type_id<int&>());
    ASSERT_EQ(type_id<int>(), type_id<int&&>());
    ASSERT_EQ(type_id<int>(), type_id<const int>());
    ASSERT_EQ(type_id<int>(), type_id<const int&>());
    ASSERT_EQ(type_id<int>(), type_id<const int&&>());
    ASSERT_EQ(type_id<int>(), type_id<volatile int>());
    ASSERT_EQ(type_id<int>(), type_id<volatile int&>());
    ASSERT_EQ(type_id<int>(), type_id<volatile int&&>());
    ASSERT_EQ(type_id<int>(), type_id<const volatile int>());
    ASSERT_EQ(type_id<int>(), type_id<const volatile int&>());
    ASSERT_EQ(type_id<int>(), type_id<const volatile int&&>());

    ASSERT_NE(type_id<char*>(), type_id<char>());
    ASSERT_NE(type_id<char*>(), type_id<const char*>());
    ASSERT_NE(type_id<char*>(), type_id<volatile char*>());
}

TEST(TypeInfoTest, IsAssignable)
{
    auto int_type_id = type_id<int>();
    auto type = int_type_id;

    ASSERT_EQ(type_id<int>(), type);

    type = type_id<char>();

    ASSERT_EQ(type_id<char>(), type);
    ASSERT_EQ(type_id<int>(), int_type_id);
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

    ASSERT_EQ(const_void_ptr_id, type_id<const void*>());
    ASSERT_EQ(const_void_ptr_const_id, type_id<const void*>());
    ASSERT_NE(int_type_id, type_id<const void*>());
    ASSERT_NE(float_type_id, type_id<const void*>());
    ASSERT_NE(forwarded_class_type_id, type_id<const void*>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<const void*>());
    ASSERT_NE(nullptr_type_id, type_id<const void*>());
    ASSERT_NE(type_id_t_type_id, type_id<const void*>());
    ASSERT_NE(uint_type_id, type_id<const void*>());

    ASSERT_NE(const_void_ptr_id, type_id<int>());
    ASSERT_NE(const_void_ptr_const_id, type_id<int>());
    ASSERT_EQ(int_type_id, type_id<int>());
    ASSERT_NE(float_type_id, type_id<int>());
    ASSERT_NE(forwarded_class_type_id, type_id<int>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<int>());
    ASSERT_NE(nullptr_type_id, type_id<int>());
    ASSERT_NE(type_id_t_type_id, type_id<int>());
    ASSERT_NE(uint_type_id, type_id<int>());

    ASSERT_NE(const_void_ptr_id, type_id<float>());
    ASSERT_NE(const_void_ptr_const_id, type_id<float>());
    ASSERT_NE(int_type_id, type_id<float>());
    ASSERT_EQ(float_type_id, type_id<float>());
    ASSERT_NE(forwarded_class_type_id, type_id<float>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<float>());
    ASSERT_NE(nullptr_type_id, type_id<float>());
    ASSERT_NE(type_id_t_type_id, type_id<float>());
    ASSERT_NE(uint_type_id, type_id<float>());

    ASSERT_NE(const_void_ptr_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(const_void_ptr_const_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(int_type_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(float_type_id, type_id<ForwardedClassDecl>());
    ASSERT_EQ(forwarded_class_type_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(nullptr_type_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(type_id_t_type_id, type_id<ForwardedClassDecl>());
    ASSERT_NE(uint_type_id, type_id<ForwardedClassDecl>());

    ASSERT_NE(const_void_ptr_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(const_void_ptr_const_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(int_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(float_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(forwarded_class_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_EQ(forwarded_class_ptr_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(nullptr_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(type_id_t_type_id, type_id<ForwardedClassDecl*>());
    ASSERT_NE(uint_type_id, type_id<ForwardedClassDecl*>());

    ASSERT_NE(const_void_ptr_id, type_id<std::nullptr_t>());
    ASSERT_NE(const_void_ptr_const_id, type_id<std::nullptr_t>());
    ASSERT_NE(int_type_id, type_id<std::nullptr_t>());
    ASSERT_EQ(nullptr_type_id, type_id<std::nullptr_t>());
    ASSERT_NE(float_type_id, type_id<std::nullptr_t>());
    ASSERT_NE(forwarded_class_type_id, type_id<std::nullptr_t>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<std::nullptr_t>());
    ASSERT_NE(type_id_t_type_id, type_id<std::nullptr_t>());
    ASSERT_NE(uint_type_id, type_id<std::nullptr_t>());

    ASSERT_NE(const_void_ptr_id, type_id<type_id_t>());
    ASSERT_NE(const_void_ptr_const_id, type_id<type_id_t>());
    ASSERT_NE(int_type_id, type_id<type_id_t>());
    ASSERT_NE(float_type_id, type_id<type_id_t>());
    ASSERT_NE(forwarded_class_type_id, type_id<type_id_t>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<type_id_t>());
    ASSERT_NE(nullptr_type_id, type_id<type_id_t>());
    ASSERT_EQ(type_id_t_type_id, type_id<type_id_t>());
    ASSERT_NE(uint_type_id, type_id<type_id_t>());

    ASSERT_NE(const_void_ptr_id, type_id<unsigned int>());
    ASSERT_NE(const_void_ptr_const_id, type_id<unsigned int>());
    ASSERT_NE(int_type_id, type_id<unsigned int>());
    ASSERT_NE(float_type_id, type_id<unsigned int>());
    ASSERT_NE(forwarded_class_type_id, type_id<unsigned int>());
    ASSERT_NE(forwarded_class_ptr_type_id, type_id<unsigned int>());
    ASSERT_NE(nullptr_type_id, type_id<unsigned int>());
    ASSERT_NE(type_id_t_type_id, type_id<unsigned int>());
    ASSERT_EQ(uint_type_id, type_id<unsigned int>());
}
