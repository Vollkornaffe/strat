#include "game/Fixed.hh"

fixed sqrt(fixed s) {
    if (s == fixed(0))
        return fixed(0);

    // Newton's method I guess
     
    fixed accuracy = fixed(1) / fixed(1 << 10);

    fixed x = s / 2; 
    fixed e;
   
    do {
        if (x == fixed(0)) return accuracy;

        fixed x2 = fixed(1) / fixed(2) * (x + s / x);

        e = (x2 - x).abs();
        x = x2;
    } while (e > accuracy);

    return x;
}

fvec3 normalize(const fvec3 &v) {
    std::cout << v << " has length " << length(v) << std::endl;
    return v / length(v);
}

fixed length(const fvec3 &v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); 
}

fquat normalize(const fquat &q) {
    fixed oneOverLength = fixed(1) / length(q);
    return fquat(q.w * oneOverLength, q.x * oneOverLength, q.y * oneOverLength, q.z * oneOverLength);
}

fvec3 axis(const fquat &q) {
    fixed tmp1 = fixed(1) - q.w * q.w;
    if (tmp1 <= fixed(0))
        return fvec3(0, 0, 1);
    fixed tmp2 = fixed(1) / sqrt(tmp1);
    return fvec3(q.x * tmp2, q.y * tmp2, q.z * tmp2);
}

fixed length(const fquat &q) {
    return sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

fvec3 cross(const fvec3 &x, const fvec3 &y) {
    return fvec3(
        x.y * y.z - y.y * x.z,
        x.z * y.x - y.z * x.x,
        x.x * y.y - y.x * x.y);
}

fixed dot(const fquat &x, const fquat &y) {
    return x.w * y.w + x.x * y.x + x.y * y.y + x.z * y.z;
}

fquat inverse(const fquat &q) {
    return glm::conjugate(q) / dot(q, q);
}

fvec3 fquatMult(const fquat &q, const fvec3 &v) {
    const fvec3 quatVector(q.x, q.y, q.z);
    const fvec3 uv(cross(quatVector, v));
    const fvec3 uuv(cross(quatVector, uv));

    return v + ((uv * q.w) + uuv) * fixed(2);
}

fvec3 fquatMult(const fvec3 &v, const fquat &q) {
	return fquatMult(glm::inverse(q), v);
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

std::ostream& operator<<(std::ostream& os, fixed f) {
    return os << f.toFloat();
}

std::ostream& operator<<(std::ostream& os, const fvec2& v) {
    return os << "fvec2(" << v.x << ", "
                          << v.y << ")";
}

std::ostream& operator<<(std::ostream& os, const fvec3& v) {
    return os << "fvec3(" << v.x << ", "
                          << v.y << ", "
                          << v.z << ")";
}
