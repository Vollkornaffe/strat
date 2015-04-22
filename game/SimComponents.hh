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

struct Ship : entityx::Component<Ship> {
    
};

struct Position : entityx::Component<Position> {
    fvec3 position;
};

#endif
