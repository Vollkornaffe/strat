#ifndef STRAT_GAME_SIM_COMPONENTS_HH
#define STRAT_GAME_SIM_COMPONENTS_HH

#include "Map.hh"
#include "Fixed.hh"
#include "common/Defs.hh"

#include <entityx/entityx.h>

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
    // Size of bounding box
    fvec3 size;

    // Properties
    fixed mass;
    fixed inertia;

    // Movement
    fvec3 position;
    fvec3 momentum;

    fvec3 velocity;
    fvec3 force;

    // Rotation
    fquat orientation;
    fvec3 angularMomentum;

    fquat spin;
    fvec3 angularVelocity;

    PhysicsState(fixed mass, fixed inertia, fvec3 position)
        : mass(mass), inertia(inertia), position(position) {
    }

    void recalculate();
};

struct Ship : entityx::Component<Ship> {
};

#endif
