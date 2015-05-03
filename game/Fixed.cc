#include "game/Fixed.hh"

fvec3 normalize(const fvec3 &v) {

}

fixed length(const fvec3 &) {

}

glm::vec3 fixedToFloat(const fvec3 &v) {
    return glm::vec3(v.x.toFloat(), v.y.toFloat(), v.z.toFloat());
}

glm::quat fixedToFloat(const fquat &q) {
    return glm::quat(q.w.toFloat(), q.x.toFloat(), q.y.toFloat(), q.z.toFloat());
}

glm::ivec3 fixedToInt(const fvec3 &v) {
    return glm::ivec3(v.x.toInt(), v.y.toInt(), v.z.toInt());
}
