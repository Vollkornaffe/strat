#ifndef STRAT_COMMON_DEFS_HH
#define STRAT_COMMON_DEFS_HH

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>

#include <glm/glm.hpp>

typedef uint16_t PlayerId; // Valid client ids are > 0
typedef uint32_t ObjectId; // Valid object ids are > 0

enum {
    PLAYER_NEUTRAL = 0
};

#endif
