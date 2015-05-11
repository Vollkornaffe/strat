#include "SimSystems.hh"

#include "SimState.hh"
#include "SimComponents.hh"

#include "util/Print.hh"

static bool waterInteraction(SimState &state, PhysicsState::Handle physicsState, const fvec3 &shipPoint, fixed tickLengthS) {
    const Map &map(state.getMap());
    Water &water(state.getWater());

    if (shipPoint.x < 0 || shipPoint.x > water.getSizeX()-1 ||
        shipPoint.y < 0 || shipPoint.y > water.getSizeY()-1) {
        physicsState->applyForce(tickLengthS * fvec3(0, 0, fixed(500)), shipPoint);
        return false;
    }

    glm::ivec3 gridPosition(fixedToInt(shipPoint));
    const GridPoint &gridPoint(map.point(Map::Pos(gridPosition)));
    WaterPoint &waterPoint(water.point(Map::Pos(gridPosition)));

    fixed waterHeight(water.fpoint(fvec2(shipPoint.x, shipPoint.y)).height);
    fixed waterVelocity(water.fpoint(fvec2(shipPoint.x, shipPoint.y)).velocity);

    fixed delta = (shipPoint.z - (fixed(gridPoint.height) + waterHeight)) / physicsState->size.z;
    if (delta < -1) delta = -1;

    // Float up as soon as partially under water
    if (delta < 0) { 
        //physicsState->momentum.z -= delta * fixed(80);
        //std::cout << "applying force " << -delta << " at point " << shipPoint << std::endl;
        physicsState->applyForce(tickLengthS * fvec3(0, 0, -delta * fixed(1000)), shipPoint);
        //std::cout << "-> " << physicsState->angularMomentum << std::endl;
    }


    // Accelerate upwards slightly with waves
    if (delta <= 0 && waterVelocity > 0) {
        //physicsState->momentum.z += waterPoint.acceleration * fixed(150);
        //physicsState->applyForce(fvec3(0, 0, waterVelocity / fixed(500)), shipPoint);
    }

    // Cause ripples in the water when falling down and hitting water
    if (physicsState->velocity.z < 0 && delta <= 0) {
        //fixed spread = fixed(1)/fixed(2);
        //waterPoint.velocity -= -delta * spread * physicsState->velocity.z;
        //physicsState->momentum.z += -delta * spread * physicsState->velocity.z;

        // ... and decrease momentum
        //physicsState->momentum.z -= fixed(10)/fixed(50) * physicsState->momentum.z;
        physicsState->applyForce(fvec3(0, 0, -fixed(10)/fixed(50) * tickLengthS * physicsState->momentum.z), shipPoint);
    }

    return delta <= 0;
}

void PhysicsSystem::tick(SimState &state, fixed tickLengthS) {
    // Playground for now

    const Map &map(state.getMap());
    Water &water(state.getWater());

    PhysicsState::Handle physicsState;
    for (auto entity : state.entities.entities_with_components(physicsState)) {
        // Friction
        physicsState->momentum -= tickLengthS * fixed(2)/fixed(5) * physicsState->momentum;
        physicsState->angularMomentum -= tickLengthS * fixed(9)/fixed(10) * physicsState->angularMomentum;
       
        // Float on water, gravitation
        glm::ivec3 gridPosition(fixedToInt(physicsState->position));
        const GridPoint &gridPoint(map.point(Map::Pos(gridPosition)));
        WaterPoint &waterPoint(water.point(Map::Pos(gridPosition)));

        fmat3 m(glm::mat3_cast(physicsState->orientation));

        fvec3 shipAxis(normalize(m * fvec3(1,0,0))),
              orthAxis(normalize(m * fvec3(0,1,0)));
        fvec3 normalAxis(normalize(cross(shipAxis, orthAxis)));
        fvec3 base = physicsState->position - (fixed(physicsState->size.z) / 2) * normalAxis;

        //std::cout << "SHIP AXIS: " << shipAxis << ", ORTH AXIS: " << orthAxis << std::endl;
        //std::cout << "NORMAL: " << normalAxis << std::endl;

        physicsState->applyForce(tickLengthS * fvec3(0, 0, -1000),
                physicsState->position - (fixed(physicsState->size.x) / 4) * shipAxis
                                       - physicsState->size.z * normalAxis);
        physicsState->applyForce(tickLengthS * fvec3(0, 0, -1000),
                physicsState->position + (fixed(physicsState->size.x) / 4) * shipAxis
                                       - physicsState->size.z * normalAxis);


        size_t inWater = 0;
        inWater += waterInteraction(state, physicsState, base + fixed(1)/fixed(2) * shipAxis * physicsState->size.x, tickLengthS);
        inWater += waterInteraction(state, physicsState, base - fixed(1)/fixed(2) * shipAxis * physicsState->size.x, tickLengthS);
        inWater += waterInteraction(state, physicsState, base + fixed(1)/fixed(2) * orthAxis * physicsState->size.y, tickLengthS);
        inWater += waterInteraction(state, physicsState, base - fixed(1)/fixed(2) * orthAxis * physicsState->size.y, tickLengthS);

        // Move
        physicsState->recalculate();

        physicsState->position += physicsState->velocity * tickLengthS;
        physicsState->orientation += physicsState->spin * tickLengthS;

        physicsState->recalculate();

        fixed waterSpeed(sqrt(physicsState->velocity.x * physicsState->velocity.x + physicsState->velocity.y * physicsState->velocity.y));
        if (inWater && waterSpeed > fixed(1)/fixed(10)) {
            waterPoint.velocity += fixed(1)/fixed(3) * inWater * (waterSpeed > 3 ? 3 : waterSpeed) * tickLengthS;
        }

        // Clip to map size
        gridPosition = fixedToInt(physicsState->position);

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

    fmat3 m(glm::mat3_cast(physicsState->orientation));
    fvec3 shipAxis(m * fvec3(1,0,0)), orthAxis(m * fvec3(0,1,0));
    fvec3 normalAxis(normalize(cross(shipAxis, orthAxis)));
    fvec3 base = physicsState->position - (fixed(physicsState->size.z) / 2) * normalAxis;

    switch (direction) { 
        case DIRECTION_FORWARD:
            //physicsState->momentum.x += 100.0f;
            physicsState->momentum += fixed(100) * shipAxis;
            //physicsState->applyForce(fvec3(0, 0, 100), base + fixed(1)/fixed(2) * shipAxis * physicsState->size.x);
            break;
        case DIRECTION_BACKWARD:
            //physicsState->momentum.x -= 100.0f;
            physicsState->momentum -= fixed(100) * shipAxis;
            //physicsState->applyForce(fvec3(0, 0, 100), base - fixed(1)/fixed(2) * shipAxis * physicsState->size.x);
            break;
        case DIRECTION_LEFT:
            physicsState->angularMomentum.z += fixed(100);
            //physicsState->applyForce(fvec3(0, 0, 1), physicsState->position + physicsState->size.x * shipAxis);
            //physicsState->applyForce(fvec3(0, 0, 100), base + fixed(1)/fixed(2) * orthAxis * physicsState->size.y);
            break;
        case DIRECTION_RIGHT:
            physicsState->angularMomentum.z += -fixed(100);
            //physicsState->applyForce(fvec3(0, 0, 100), base - fixed(1)/fixed(2) * orthAxis * physicsState->size.y);
            break;
    }
}

void CopyPhysicsStateSystem::tick(SimState &state) {
    PreviousPhysicsState::Handle previousPhysicsState;
    PhysicsState::Handle physicsState;
    for (auto entity : state.entities.entities_with_components(previousPhysicsState, physicsState)) {
        previousPhysicsState->state = *physicsState.get();
    }
}

void ShipSystem::tick(SimState &state, fixed tickLengthS) {
}
