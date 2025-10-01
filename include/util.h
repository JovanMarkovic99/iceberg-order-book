#pragma once

#ifndef ALWAYS_INLINE
  #if defined(__clang__) || defined(__GNUC__)
    #define ALWAYS_INLINE inline __attribute__((always_inline))
  #elif defined(_MSC_VER)
    #define ALWAYS_INLINE __forceinline
  #else
    #define ALWAYS_INLINE inline
  #endif
#endif
