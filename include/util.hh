#pragma once

#include <cstdint>

static inline uint64_t rdtsc()
{
  uint32_t lo, hi;
  asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
  return static_cast<uint64_t>(hi) << 32 | lo;
}

// EOF