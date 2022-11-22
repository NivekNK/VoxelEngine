#pragma once

#include <cstdint>

// Unsigned int types
typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

// Signed int types
typedef std::int8_t i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

// Floating poit types
typedef float f32;
typedef double f64;

#define NK_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define NK_MIN(a,b) (((a) < (b)) ? (a) : (b))
