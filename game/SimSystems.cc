#include "SimSystems.hh"

#include "SimState.hh"
#include "SimComponents.hh"

void ShipSystem::accelerate(SimState &state, PlayerId player, Direction direction) {
    Ship::Handle ship(state.getPlayer(player).ship.component<Ship>());
    switch (direction) { 
        case DIRECTION_FORWARD:
            ship->velocity.x += 1.0f;
            break;
        case DIRECTION_BACKWARD:
            ship->velocity.x -= 1.0f;
            break;

        case DIRECTION_LEFT:
            ship->angularVelocity.x += 1.0f;
    }
}

void ShipSystem::tick(SimState &state, Fixed tickLengthS) {
    const Map &map(state.getMap());
    const Water &water(state.getWater());

    Ship::Handle ship;
    for (auto entity : state.entities.entities_with_components(ship)) {
        // Move
        ship->position += ship->velocity * tickLengthS;

        // Clip to map size
        glm::uvec3 gridPosition(ship->position);
        if (gridPosition.x < 0) ship->position.x = 0;
        if (gridPosition.x > map.getSizeX()-1) ship->position.x = map.getSizeX()-1;
        if (gridPosition.y < 0) ship->position.y = 0;
        if (gridPosition.y > map.getSizeY()-1) ship->position.y = map.getSizeY()-1;

        gridPosition = ship->position;

        // Float on water
        const GridPoint &gridPoint(map.point(Map::Pos(gridPosition)));
        const WaterPoint &waterPoint(water.point(Map::Pos(gridPosition)));

        ship->position.z = gridPoint.height + waterPoint.height;
    }
}
