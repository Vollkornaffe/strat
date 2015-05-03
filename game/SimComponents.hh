#ifndef STRAT_GAME_SIM_COMPONENTS_HH
#define STRAT_GAME_SIM_COMPONENTS_HH

#include "Map.hh"
#include "Math.hh"
#include "common/Defs.hh"

#include <entityx/entityx.h>
#include <Fixed.hh>

#include <algorithm>

using entityx::Entity;

struct GameObject : entityx::Component<GameObject> {
    GameObject(PlayerId owner, ObjectId id)
        : owner(owner),
          id(id) {
    }

    PlayerId getOwner() const { return owner; }
    ObjectId getId() const { return id; }

private:
    PlayerId owner;
    ObjectId id;
};

struct PhysicsState : entityx::Component<PhysicsState> {
    // Movement
    fvec3 position;
    fvec3 momentum;

    fvec3 velocity;
    fvec3 force;

    Fixed mass;

    // Rotation
    fquat orientation;
    fvec3 angularMomentum;

    fquat spin;
    fvec3 angularVelocity;

    Fixed inertia;

    PhysicsState(fvec3 position)
        : position(position) {
    }

    void recalculate();
};

struct Ship : entityx::Component<Ship> {
};

#endif
