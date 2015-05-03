#include "game/SimComponents.hh"

void PhysicsState::recalculate() {
    velocity = momentum / mass;

    //angularVelocity = angularMomentum / inertia;

    //orientation = glm::normalize(orientation);

    //fquat q(Fixed(0), angularVelocity);

    //spin = Fixed(1)/Fixed(2) * q * orientation;
}
