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

    // Rotation
    fquat orientation;
    fvec3 angularMomentum;

    fquat spin;
    fvec3 angularVelocity;

    PhysicsState() {
    }

    PhysicsState(fvec3 size, fixed mass, fixed inertia, fvec3 position)
        : size(size), mass(mass), inertia(inertia), position(position) {
    }

    void recalculate();
    void applyForce(const fvec3 &force, const fvec3 &point);

    static PhysicsState interpolate(const PhysicsState &, const PhysicsState &, fixed);
};

template<typename T>
struct PreviousState : entityx::Component<PreviousState<T>> {
    T state;
};

typedef PreviousState<PhysicsState> PreviousPhysicsState;

struct Ship : entityx::Component<Ship> {
    fixed rudder;
};

#endif
