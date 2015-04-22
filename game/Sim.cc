#include "Sim.hh"

#include "Map.hh"
#include "util/Profiling.hh"

Sim::Sim(const GameSettings &settings)
    : state(settings) {
}

void Sim::runTick(const std::vector<Order> &orders) {
    PROFILE(tick);

    for (auto &order : orders) {
        if (state.isOrderValid(order))
            state.runOrder(order);
    }

    state.tick();

    {
        PROFILE(map);
        state.getMap().tick(state.entities, state.getTickLengthS());
    }

    {
        PROFILE(water);
        state.getMap().waterTick(state.getTickLengthS());
    }

    {
        PROFILE(objects);
    }
}

const SimState &Sim::getState() const {
    return state;
}
