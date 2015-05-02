#ifndef STRAT_GAME_WATER_HH
#define STRAT_GAME_WATER_HH

#include "Map.hh"

#include <Fixed.hh>

#include <cassert>
#include <vector>

struct WaterPoint {
    Fixed height, velocity;

    WaterPoint()
        : height(100), velocity() {
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

    void splash(const Map::Pos &, float speed);

    void tick(Fixed tickLengthS);

private:
    void spring(Fixed tickLengthS, WaterPoint &point);
    void propagate(Fixed tickLengthS, 
                   size_t fromX, size_t fromY,
                   size_t toX, size_t toY);

    const Map &map;

    size_t sizeX, sizeY;

    std::vector<WaterPoint> points[2];
    std::vector<WaterPoint> *newBuffer, *oldBuffer;

    size_t numPasses;

    Fixed dampening;
    Fixed tension;
    Fixed spread;
};

#endif
