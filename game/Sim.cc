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

}

const SimState &Sim::getState() const {
    return state;
}
