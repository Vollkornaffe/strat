#include "game/SimComponents.hh"

PhysicsState::recalculate() {
    velocity = momentum / mass;

    angularVelocity = angularMomentum / inertia;

    orientation.normalize();

    fquat q(0, angularVelocity);

    spin = Fixed(1)/Fixed(2) * q * orientation;
}
