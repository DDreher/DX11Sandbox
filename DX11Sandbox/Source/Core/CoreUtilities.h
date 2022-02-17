#pragma once
#include "AssertMacros.h"

// Convenient hash enable - See https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void HashCombine(std::size_t& seed) {}

template <typename T, typename... Rest>
inline void HashCombine(std::size_t& seed, const T& v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    HashCombine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                HashCombine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }
