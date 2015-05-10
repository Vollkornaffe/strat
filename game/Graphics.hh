#ifndef STRAT_GAME_GRAPHICS_HH
#define STRAT_GAME_GRAPHICS_HH

#include "Config.hh"
#include "Math.hh"
#include "Sim.hh"
#include "Map.hh"
#include "Input.hh"

#include <entityx/entityx.h>
#include <GL/glew.h>

#include "opengl/OBJ.hh"
#include "opengl/TextureManager.hh"

struct Map;
struct InterpState;

struct RenderShipSystem {
    RenderShipSystem(const Map &map, const Input &input, opengl::TextureManager &textures)
        : map(map), input(input),
          shipObj("data/stupidShip/stupidShip2.obj", textures) {
    }

    void render(entityx::EntityManager &entities);

private:
    const Map &map;
    const Input &input;

    opengl::OBJ shipObj;
};

void setupGraphics(const Config &, const Input::View &);

#endif
