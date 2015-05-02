#ifndef STRAT_GAME_TERRAIN_HH
#define STRAT_GAME_TERRAIN_HH

#include "Math.hh"
#include "Map.hh"
#include "Water.hh"

#include <GL/glew.h>

struct TerrainPatch;

// Draws the map
struct TerrainMesh {
    TerrainMesh(const Map &, const Water &water, const Map::Pos &patchSize);
    ~TerrainMesh();

    void update();
    void draw();
    void drawWater();

    bool intersectWithRay(const Ray &ray, Map::Pos &point, float &t) const;

private:
    void init();

    glm::vec3 color(size_t height);

    const Map &map;
    const Water &water;

    std::vector<TerrainPatch *> patches;
};

#endif
