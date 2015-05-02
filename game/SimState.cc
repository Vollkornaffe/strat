#include "SimState.hh"

#include "SimComponents.hh"
#include "util/Profiling.hh"

#include <cstdlib>

PlayerState::PlayerState(const PlayerInfo &info)
    : info(info) {
}

SimState::SimState(const GameSettings &settings)
    : settings(settings),
      map(settings.mapW, settings.mapH),
      water(map),
      players(playersFromSettings(settings)),
      entityCounter(0),
      time(0) {
    for (auto &player : settings.players) {
        size_t x = rand() % settings.mapW, y = rand() % settings.mapH;
        getPlayer(player.id).ship = addShip(player.id, fvec2(Fixed(x), Fixed(y)));
    }

    for (size_t i = 0; i < 10; i++) {
        size_t x = rand() % settings.mapW, y = rand() % settings.mapH;
        size_t v = rand() % 100;
        water.splash(Map::Pos(x, y), v);
    }
    for (int i = 0; i < 10; i++)
        water.tick(Fixed(1)/Fixed(10));
}

bool SimState::isOrderValid(const Order &order) const {
    if (players.find(order.player) == players.end())
        return false;

    switch (order.type) {
        case Order::ACCELERATE:
            return true;

        default:
            return false;
    }
}

void SimState::runOrder(const Order &order) {
    assert(isOrderValid(order));

    static int yolo = 0;

    switch (order.type) {
        case Order::ACCELERATE: {
            shipSystem.accelerate(*this, order.player, order.accelerate.direction);

            /*size_t x1 = rand() % settings.mapW, y1 = rand() % settings.mapH;
            size_t x2 = rand() % settings.mapW, y2 = rand() % settings.mapH;
            for (size_t x = x1; x <= x1 + 50 && x < map.getSizeX(); x++)
                for (size_t y = y1; y <= y1 + 50 && y < map.getSizeY(); y++)
                        water.splash(Map::Pos(x, y), 5.0f + rand() % 10);*/

            /*for (size_t x = 0; x < map.getSizeX(); x++)
                water.splash(Map::Pos(x, 0), 15.0f);*/

            break;
        }

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
    entity.assign<Ship>(fvec3(position, Fixed(0)));

    return entity;
}

void SimState::tick() {
    Fixed tickLengthS = getTickLengthS();

    time += tickLengthS;

    {
        PROFILE(map);
        map.tick(tickLengthS);
    }

    {
        PROFILE(water);
        water.tick(tickLengthS);
    }

    {
        PROFILE(objects);
        shipSystem.tick(*this, tickLengthS);
    }
} 

SimState::PlayerMap SimState::playersFromSettings(const GameSettings &settings) {
    SimState::PlayerMap players;

    for (auto info : settings.players) {
        players.emplace(info.id, info);
    }

    return players;
}
