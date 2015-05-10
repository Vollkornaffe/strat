#ifndef STRAT_GAME_SIM_STATE_HH
#define STRAT_GAME_SIM_STATE_HH

#include "Map.hh"
#include "Water.hh"
#include "SimSystems.hh"
#include "Fixed.hh"
#include "common/GameSettings.hh"
#include "common/Order.hh"

#include <entityx/entityx.h>
#include <map>

using entityx::Entity;

struct PlayerState {
    PlayerState(const PlayerInfo &info);

    Entity ship;

private:
    const PlayerInfo &info;
};

// Contains all the relevant information about the game state,
// so that the next state can be calculated deterministically.
//
// This state includes:
// - the map, which stores the heights of the grid points and the water state,
// - information about players, and
// - the game entities which are managed as components using EntityX
//
// SimState also offers several functions to modify the game state.
struct SimState : entityx::EntityX {
    SimState(const GameSettings &);

    bool isOrderValid(const Order &) const;
    void runOrder(const Order &);
    
    Map &getMap() { return map; }
    const Map &getMap() const { return map; }

    Water &getWater() { return water; }
    const Water &getWater() const { return water; }

    PlayerState &getPlayer(PlayerId);
    const PlayerState &getPlayer(PlayerId) const;

    Entity getGameObject(ObjectId) const;

    // Tick length in seconds
    fixed getTickLengthS() const;

    // Time elapsed in simulation in seconds
    fixed getTimeS() const { return time; }

    Entity addShip(PlayerId owner, const fvec2 &position);

    void tick();

private:
    typedef std::map<PlayerId, PlayerState> PlayerMap;

    const GameSettings &settings;

    Map map;
    Water water;
    
    PlayerMap players;

    size_t entityCounter;

    fixed time;

    CopyPhysicsStateSystem copyPhysicsStateSystem;
    PhysicsSystem physicsSystem;
    ShipSystem shipSystem;

    static PlayerMap playersFromSettings(const GameSettings &);
};

#endif
