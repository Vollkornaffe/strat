#ifndef STRAT_GAME_SIM_SYSTEMS_HH
#define STRAT_GAME_SIM_SYSTEMS_HH

#include "SimComponents.hh"
#include "Fixed.hh"
#include "common/Order.hh"

#include <entityx/entityx.h>

struct SimState;

struct PhysicsSystem {
    void tick(SimState &, fixed tickLengthS);
};

struct ShipSystem {
    void accelerate(SimState &, PlayerId, Direction);
    void tick(SimState &, fixed tickLengthS);
};

#endif
