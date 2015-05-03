#include "SimSystems.hh"

#include "SimState.hh"
#include "SimComponents.hh"

void PhysicsSystem::tick(SimState &state, fixed tickLengthS) {
    const Map &map(state.getMap());
    const Water &water(state.getWater());

    PhysicsState::Handle physicsState;
    for (auto entity : state.entities.entities_with_components(physicsState)) {
        physicsState->recalculate();

        // Move
        physicsState->position += physicsState->velocity * tickLengthS;

        // Clip to map size
        glm::ivec3 gridPosition(fixedToInt(physicsState->position));
        if (gridPosition.x < 0) {
            physicsState->position.x = 0;
            physicsState->velocity.x = 0;
        }
        if (gridPosition.x > (int)map.getSizeX()-1) { 
            physicsState->position.x = map.getSizeX()-1;
            physicsState->velocity.x = 0;
        }
        if (gridPosition.y < 0) {
            physicsState->position.y = 0;
            physicsState->velocity.y = 0;
        }
        if (gridPosition.y > (int)map.getSizeY()-1) {
            physicsState->position.y = map.getSizeY()-1;
            physicsState->velocity.y = 0;
        }

        gridPosition = fixedToInt(physicsState->position);

        // Float on water
        const GridPoint &gridPoint(map.point(Map::Pos(gridPosition)));
        const WaterPoint &waterPoint(water.point(Map::Pos(gridPosition)));

        physicsState->position.z = fixed(gridPoint.height) + waterPoint.height;
    }
}

void ShipSystem::accelerate(SimState &state, PlayerId player, Direction direction) {
    Entity shipEntity(state.getPlayer(player).ship);
    PhysicsState::Handle physicsState(shipEntity.component<PhysicsState>());

    switch (direction) { 
        case DIRECTION_FORWARD:
            physicsState->momentum.x += 100.0f;
            break;
        case DIRECTION_BACKWARD:
            physicsState->momentum.x -= 100.0f;
            break;
        case DIRECTION_LEFT:
            physicsState->angularMomentum.x += 1.0f;
            break;
        case DIRECTION_RIGHT:
            physicsState->angularMomentum.x += -1.0f;
            break;
    }
}

void ShipSystem::tick(SimState &state, fixed tickLengthS) {
}
