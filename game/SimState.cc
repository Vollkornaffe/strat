#include "SimState.hh"

#include "SimComponents.hh"

#include <cstdlib>

PlayerState::PlayerState(const PlayerInfo &info)
    : info(info) {
}

SimState::SimState(const GameSettings &settings)
    : settings(settings),
      map(settings.mapW, settings.mapH),
      players(playersFromSettings(settings)),
      entityCounter(0),
      time(0) {
}

bool SimState::isOrderValid(const Order &order) const {
    switch (order.type) {
        default:
            return false;
    }
}

void SimState::runOrder(const Order &order) {
    assert(isOrderValid(order));

    switch (order.type) {
        default:
            return;
    }
}

PlayerState &SimState::getPlayer(PlayerId id) {
    auto it = players.find(id);
    assert(it != players.end());
    return it->second;
}

const PlayerState &SimState::getPlayer(PlayerId id) const {
    auto it = players.find(id);
    assert(it != players.end());
    return it->second;
}

entityx::Entity SimState::getGameObject(ObjectId id) const {
    entityx::Entity result;
    GameObject::Handle gameObject;

    auto ents = const_cast<entityx::EntityManager *>(&entities); // I'm sorry...

    // TODO: Maybe switch to a std::map<ObjectId, Entity> at some point
    for (auto entity : ents->entities_with_components(gameObject)) {
        if (gameObject->getId() == id) {
            assert(!result);
            result = entity;
        }
    }

    return result;
}


Fixed SimState::getTickLengthS() const {
    return Fixed(settings.tickLengthMs) / Fixed(1000);
}

entityx::Entity SimState::addShip(PlayerId owner, const fvec2 &position) {
    entityx::Entity entity = entities.create();
    entity.assign<GameObject>(owner, ++entityCounter);

    return entity;
}

void SimState::tick() {
    time += getTickLengthS();
}

SimState::PlayerMap SimState::playersFromSettings(const GameSettings &settings) {
    SimState::PlayerMap players;

    for (auto info : settings.players) {
        //players.emplace(info.id, info);
    }

    return players;
}
