#include "SimSystems.hh"

#include "SimState.hh"
#include "SimComponents.hh"

void PhysicsSystem::tick(SimState &state, fixed tickLengthS) {
    // Playground for now

    const Map &map(state.getMap());
    Water &water(state.getWater());

    PhysicsState::Handle physicsState;
    for (auto entity : state.entities.entities_with_components(physicsState)) {
        // Friction
        physicsState->momentum -= fixed(1)/fixed(50) * physicsState->momentum;
        physicsState->angularMomentum -= fixed(1)/fixed(50) * physicsState->angularMomentum;
       
        // Float on water, gravitation
        glm::ivec3 gridPosition(fixedToInt(physicsState->position));
        const GridPoint &gridPoint(map.point(Map::Pos(gridPosition)));
        WaterPoint &waterPoint(water.point(Map::Pos(gridPosition)));

        physicsState->momentum.z -= fixed(50);

        // How far above the water is the center of the boat?
        fixed delta = physicsState->position.z - (fixed(gridPoint.height) + waterPoint.height);

        // Float up as soon as partially under water
        if (delta < physicsState->size.z / 2) { 
            if (delta < 1) delta = -1;
            physicsState->momentum.z -= delta * fixed(80);
        }

        // Accelerate upwards slightly with waves
        if (delta <= physicsState->size.z && waterPoint.acceleration > 0) {
            physicsState->momentum.z += waterPoint.acceleration * fixed(150);
        }

        // Cause ripples in the water when falling down and hitting water
        if (physicsState->momentum.z < 0 && delta < physicsState->size.z / fixed(2)) {
            waterPoint.velocity += physicsState->momentum.z / (2 * physicsState->mass);

            // ... and decrease momentum
            physicsState->momentum.z -= fixed(10)/fixed(50) * physicsState->momentum.z;
        }

        // Move
        physicsState->recalculate();

        physicsState->position += physicsState->velocity * tickLengthS;
        physicsState->orientation += physicsState->spin * tickLengthS;

        fixed waterSpeed(sqrt(physicsState->velocity.x * physicsState->velocity.x + physicsState->velocity.y * physicsState->velocity.y));
        if (waterSpeed > fixed(1)/fixed(10)) {
            waterPoint.velocity += fixed(1) / fixed(5) * (waterSpeed > 3 ? 3 : waterSpeed); //fixed(1)/fixed(2);
        }

        // Clip to map size
        if (gridPosition.x < 0) {
            physicsState->position.x = 0;
            physicsState->momentum.x = 0;
        }
        if (gridPosition.x > (int)map.getSizeX()-1) { 
            physicsState->position.x = map.getSizeX()-1;
            physicsState->momentum.x = 0;
        }
        if (gridPosition.y < 0) {
            physicsState->position.y = 0;
            physicsState->momentum.y = 0;
        }
        if (gridPosition.y > (int)map.getSizeY()-1) {
            physicsState->position.y = map.getSizeY()-1;
            physicsState->momentum.y = 0;
        }

        gridPosition = fixedToInt(physicsState->position);
        assert(gridPosition.x >= 0 && gridPosition.x < map.getSizeX());
        assert(gridPosition.y >= 0 && gridPosition.y < map.getSizeY());

        //physicsState->position.z = fixed(gridPoint.height) + waterPoint.height;
    }
}

void ShipSystem::accelerate(SimState &state, PlayerId player, Direction direction) {
    Entity shipEntity(state.getPlayer(player).ship);
    PhysicsState::Handle physicsState(shipEntity.component<PhysicsState>());

    //fvec3 shipAxis(axis(physicsState->orientation));
    fvec3 shipAxis(fquatMult(fquatMult(physicsState->orientation, fvec3(1, 0, 0)),
                             glm::conjugate(physicsState->orientation)));
    fmat3 m(glm::mat3_cast(physicsState->orientation));
    shipAxis = m * fvec3(1,0,0);
    std::cout << shipAxis.x.toFloat() << "," << shipAxis.y.toFloat() << "," << shipAxis.z.toFloat() << std::endl;

    switch (direction) { 
        case DIRECTION_FORWARD:
            //physicsState->momentum.x += 100.0f;
            physicsState->momentum += fixed(300) * shipAxis;
            break;
        case DIRECTION_BACKWARD:
            //physicsState->momentum.x -= 100.0f;
            physicsState->momentum -= fixed(300) * shipAxis;
            break;
        case DIRECTION_LEFT:
            physicsState->angularMomentum.z += fixed(100);
            break;
        case DIRECTION_RIGHT:
            physicsState->angularMomentum.z += -fixed(100);
            break;
    }
}

void ShipSystem::tick(SimState &state, fixed tickLengthS) {
}
