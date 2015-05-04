#ifndef STRAT_GAME_FIXED_HPP
#define STRAT_GAME_FIXED_HPP

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Fixed.hh>

typedef glm::detail::tvec2<fixed, glm::highp> fvec2;
typedef glm::detail::tvec3<fixed, glm::highp> fvec3;
typedef glm::detail::tquat<fixed, glm::highp> fquat;
typedef glm::detail::tmat3x3<fixed, glm::highp> fmat3;
typedef glm::detail::tmat4x4<fixed, glm::highp> fmat4;

fixed sqrt(fixed);

// Below we implement some functions on quaternions and vectors using fixed point math.
// Unfortunately, the standard glm implementations do not work with fixed point floats.

fvec3 normalize(const fvec3 &);
fixed length(const fvec3 &);

fquat normalize(const fquat &);
fixed length(const fquat &);
fvec3 axis(const fquat &q);
fvec3 cross(const fvec3 &a, const fvec3 &b);
fixed dot(const fquat &x, const fquat &y);
fquat inverse(const fquat &q);
fvec3 fquatMult(const fquat &q, const fvec3 &v);
fvec3 fquatMult(const fvec3 &v, const fquat &q);

glm::vec3 fixedToFloat(const fvec3 &);
glm::quat fixedToFloat(const fquat &);

glm::ivec3 fixedToInt(const fvec3 &);

#endif
