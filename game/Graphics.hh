#ifndef STRAT_GAME_GRAPHICS_HH
#define STRAT_GAME_GRAPHICS_HH

#include "Config.hh"
#include "util/Math.hh"
#include "Sim.hh"
#include "Map.hh"
#include "Input.hh"

#include <entityx/entityx.h>
#include <GL/glew.h>

#include "opengl/OBJ.hh"
#include "opengl/TextureManager.hh"
#include "opengl/ProgramManager.hh"

struct Map;
struct InterpState;

struct RenderShipSystem {
    RenderShipSystem(const Input &input,
                     opengl::TextureManager &textures)
        : input(input), textures(textures) {
    }

    void load();

    void render(entityx::EntityManager &entities, const InterpState &);

private:
    const Input &input;
    opengl::TextureManager &textures;

    std::unique_ptr<opengl::OBJ> shipObj;
};

struct DebugRenderPhysicsStateSystem {
    DebugRenderPhysicsStateSystem() {
    }

    void load();

    void render(entityx::EntityManager &entities, const InterpState &);
};

struct Graphics {
    Graphics(const Config &,
             const Input &,
             opengl::TextureManager &,
             opengl::ProgramManager &);

    void load();
    void initTerrain(const Map &, const Water &);

    void setup(const Input::View &);
    void render(entityx::EntityManager &entities, const InterpState &);

    void update();

private:
    Config config;
    const Input &input;

    opengl::TextureManager &textures;
    opengl::ProgramManager &programs;

    RenderShipSystem renderShipSystem;
    DebugRenderPhysicsStateSystem debugRenderPhysicsStateSystem;

    std::unique_ptr<TerrainMesh> terrain;
};


#endif
