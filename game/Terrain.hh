#ifndef STRAT_GAME_TERRAIN_HH
#define STRAT_GAME_TERRAIN_HH

#include "Math.hh"
#include "Map.hh"
#include "Water.hh"
#include "opengl/ProgramManager.hh"

#include <GL/glew.h>

struct TerrainPatch;
struct InterpState;

// Draws the map & water
struct TerrainMesh {
    TerrainMesh(const Map &, const Water &water, const Map::Pos &patchSize,
                opengl::ProgramManager &programs);
    ~TerrainMesh();

    void update();
    void draw();
    void drawWater(const InterpState &interp);

    bool intersectWithRay(const Ray &ray, Map::Pos &point, float &t) const;

private:
    void init();

    glm::vec3 color(size_t height);

    const Map &map;
    const Water &water;

    std::vector<TerrainPatch *> patches;

    opengl::Program *waterProgram;
};

#endif
