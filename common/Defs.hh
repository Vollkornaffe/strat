#ifndef STRAT_COMMON_DEFS_HH
#define STRAT_COMMON_DEFS_HH

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <Fixed.hh>

typedef uint16_t PlayerId; // Valid client ids are > 0
typedef uint32_t ObjectId; // Valid object ids are > 0

enum {
    PLAYER_NEUTRAL = 0
};

typedef glm::detail::tvec3<Fixed, glm::highp> fvec3;
typedef glm::detail::tvec2<Fixed, glm::highp> fvec2;

typedef glm::detail::tquat<Fixed, glm::highp> fquat;

size_t sqDistance(const glm::uvec2 &a, const glm::uvec2 &b);
Fixed manhattanDistance(const fvec2 &a, const fvec2 &b);

bool pointInRange(size_t range,
                  const glm::uvec2 &center, const glm::uvec2 &point);

#endif
