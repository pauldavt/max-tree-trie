#pragma once

#include <cstdlib>
#include <cstdint>
#include <algorithm>

#define ALWAYS_INLINE __attribute__((always_inline))
#define ALWAYS_INLINE_F ALWAYS_INLINE inline
#define NO_INLINE __attribute__ ((noinline))
#define RESTRICT __restrict__

#define L_RET(...) -> __VA_ARGS__
#if defined(__clang__) || __GNUC__ < 9 || (__GNUC__ == 9 && __GNUC_MINOR__ <= 1)
#  define ALWAYS_INL_L(...) ALWAYS_INLINE -> __VA_ARGS__
#else
#  define ALWAYS_INL_L(...) -> __VA_ARGS__ ALWAYS_INLINE
#endif

#define NAMESPACE_MXT namespace mxt {
#define NAMESPACE_MXT_END }

NAMESPACE_MXT

template <typename A, typename B>
constexpr A div_roundup(A const& a, B const& b)
{
  A result = a / b;

  if (a % b)
  {
    ++result;
  }

  return result;
}

NAMESPACE_MXT_END
