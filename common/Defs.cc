#include "common/Defs.hh"

#include <algorithm>

size_t sqDistance(const glm::uvec2 &a, const glm::uvec2 &b) {
    size_t x = std::max(a.x, b.x) - std::min(a.x, b.x);
    size_t y = std::max(a.y, b.y) - std::min(a.y, b.y);
    return x * x + y * y;
}

Fixed manhattanDistance(const fvec2 &a, const fvec2 &b) {
    return (a.x - b.x).abs() + (a.y - b.y).abs();
}

bool pointInRange(size_t range,
                  const glm::uvec2 &center, const glm::uvec2 &point) {
    return sqDistance(center, point) <= range * range;
}
