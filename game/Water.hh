#ifndef STRAT_GAME_WATER_HH
#define STRAT_GAME_WATER_HH

#include "Map.hh"
#include "Fixed.hh"

#include <cassert>
#include <vector>

struct WaterPoint {
    fixed height, velocity;
    fixed acceleration; // acceleration that was applied in the last water tick

    WaterPoint()
        : height(100), velocity(), acceleration() {
    }
};

struct Water {
    Water(const Map &);

    const WaterPoint &point(size_t x, size_t y) const {
        assert(x < sizeX);
        assert(y < sizeY);
        return (*oldBuffer)[y * sizeX + x];
    }
    
    WaterPoint &point(size_t x, size_t y) {
        assert(x < sizeX);
        assert(y < sizeY);
        return (*oldBuffer)[y * sizeX + x];
    }

    WaterPoint &point(const Map::Pos &p) {
        return point(p.x, p.y);
    }

    const WaterPoint &point(const Map::Pos &p) const {
        return point(p.x, p.y);
    }

    void splash(const Map::Pos &, fixed speed);

    void tick(fixed tickLengthS);

private:
    void spring(fixed tickLengthS, WaterPoint &point);
    void propagate(fixed tickLengthS, 
                   size_t fromX, size_t fromY,
                   size_t toX, size_t toY);

    const Map &map;

    size_t sizeX, sizeY;

    std::vector<WaterPoint> points[2];
    std::vector<WaterPoint> *newBuffer, *oldBuffer;

    size_t numPasses;

    fixed dampening;
    fixed tension;
    fixed spread;
};

#endif
