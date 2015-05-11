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
        getPlayer(player.id).ship = addShip(player.id, fvec2(fixed(x), fixed(y)));
    }

    /*for (size_t i = 0; i < 10; i++) {
        size_t x = rand() % settings.mapW, y = rand() % settings.mapH;
        size_t v = rand() % 100;
        water.splash(Map::Pos(x, y), v);
    }
    for (int i = 0; i < 10; i++)
        water.tick(fixed(1)/fixed(10));*/
    size_t x1 = 120, y1 = 120; 
    for (size_t x = x1; x <= x1 + 16; x++) {
        for (size_t y = y1; y <= y1 + 16; y++) {
            fixed distance = sqrt(fixed(((x-128)*(x-128) + (y-128)*(y-128))));
            water.splash(Map::Pos(x, y), fixed(1) / (1 + distance) * fixed(100));
        }
    }
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

    switch (order.type) {
        case Order::ACCELERATE: {
            shipSystem.accelerate(*this, order.player, order.accelerate.direction);

            /*size_t x1 = rand() % settings.mapW, y1 = rand() % settings.mapH;
            size_t x2 = rand() % settings.mapW, y2 = rand() % settings.mapH;
            for (size_t x = x1; x <= x1 + 10 && x < map.getSizeX(); x++)
                for (size_t y = y1; y <= y1 + 10 && y < map.getSizeY(); y++)
                        water.splash(Map::Pos(x, y), 50);*/

            for (size_t y = 0; y < 10; y++)
                for (size_t x = 0; x < map.getSizeX(); x++)
                    water.splash(Map::Pos(x, y), fixed(1));

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


fixed SimState::getTickLengthS() const {
    return fixed(settings.tickLengthMs) / fixed(1000);
}

entityx::Entity SimState::addShip(PlayerId owner, const fvec2 &position) {
    entityx::Entity entity = entities.create();
    entity.assign<GameObject>(owner, ++entityCounter);
    entity.assign<PreviousPhysicsState>();
    entity.assign<PhysicsState>(fvec3(3,1,1), fixed(100), fixed(500), fvec3(position, fixed(100)));
    entity.assign<Ship>();

    return entity;
}

void SimState::tick() {
    fixed tickLengthS = getTickLengthS();

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
        copyPhysicsStateSystem.tick(*this);
        physicsSystem.tick(*this, tickLengthS);
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
