#pragma once

#include <cstddef>

// https://stackoverflow.com/questions/12811330/c-compile-time-offsetof-inside-a-template
template <typename T, typename M> M get_member_type(M T::*);
template <typename T, typename M> T get_class_type(M T::*);

template <typename T,
        typename R,
        R T::*M
>
constexpr std::size_t offset_of()
{
    return reinterpret_cast<std::size_t>(&(((T*)0)->*M));
}

// Compile-time Offset in class ( Usage: OFFSET_OF(&classname::x) )
#define OFFSET_OF(m) offset_of<decltype(get_class_type(m)), decltype(get_member_type(m)), m>()
