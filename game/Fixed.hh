#ifndef STRAT_GAME_FIXED_HPP
#define STRAT_GAME_FIXED_HPP

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Fixed.hh>

typedef glm::detail::tvec3<fixed, glm::highp> fvec3;
typedef glm::detail::tvec2<fixed, glm::highp> fvec2;
typedef glm::detail::tquat<fixed, glm::highp> fquat;

fvec3 normalize(const fvec3 &);
fixed length(const fvec3 &);

glm::vec3 fixedToFloat(const fvec3 &);
glm::quat fixedToFloat(const fquat &);

glm::ivec3 fixedToInt(const fvec3 &);

#endif
