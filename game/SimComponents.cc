#include "game/SimComponents.hh"
#include "util/Math.hh"

void PhysicsState::recalculate() {
    velocity = momentum / mass;
    angularVelocity = angularMomentum / inertia;

    orientation = normalize(orientation);

    fquat q(fixed(0), angularVelocity);
    spin = fixed(1)/fixed(2) * q * orientation;
}

void PhysicsState::applyForce(const fvec3 &force, const fvec3 &point) {
    momentum += force;

    //std::cout << force << " x " << point - position << " = " << cross(force, point-position) << std::endl;
    //std::cout << position << std::endl;
    angularMomentum += cross(force, position - point);
}

PhysicsState PhysicsState::interpolate(const PhysicsState &a, const PhysicsState &b, fixed t) {
    assert(t >= 0 && t <= 1);

    PhysicsState r;

    r.size = a.size;

    r.mass = a.mass;
    r.inertia = a.inertia;

    r.position = lerp(a.position, b.position, t);
    r.momentum = a.momentum;

    r.velocity = a.velocity;

    // HACK: We use floats here to slerp the orientation quaternion
    glm::quat orientation(glm::slerp(fixedToFloat(a.orientation),
                                     fixedToFloat(b.orientation),
                                     t.toFloat()));
    r.orientation = fquat(fixed::fromFloat(orientation.w),
                          fixed::fromFloat(orientation.x),
                          fixed::fromFloat(orientation.y),
                          fixed::fromFloat(orientation.z));
    r.angularMomentum = a.angularMomentum;

    r.spin = a.spin;
    r.angularVelocity = a.angularVelocity;

    return r;
}
