#include "game/SimComponents.hh"

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
