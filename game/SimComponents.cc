#include "game/SimComponents.hh"

void PhysicsState::recalculate() {
    velocity = momentum / mass;
    angularVelocity = angularMomentum / inertia;

    orientation = normalize(orientation);

    fquat q(fixed(0), angularVelocity);
    spin = fixed(1)/fixed(2) * q * orientation;
}
