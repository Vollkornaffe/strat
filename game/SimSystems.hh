#ifndef STRAT_GAME_SIM_SYSTEMS_HH
#define STRAT_GAME_SIM_SYSTEMS_HH

#include <entityx/entityx.h>

#include "SimComponents.hh"
#include "common/Order.hh"

struct SimState;

struct PhysicsSystem {
    void tick(SimState &, Fixed tickLengthS);
};

struct ShipSystem {
    void accelerate(SimState &, PlayerId, Direction);
    void tick(SimState &, Fixed tickLengthS);
};

#endif
