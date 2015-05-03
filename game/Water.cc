#include "Water.hh"

Water::Water(const Map &map)
    : map(map),
      sizeX(map.getSizeX()), sizeY(map.getSizeY()),
      points{std::vector<WaterPoint>(sizeX * sizeY),
             std::vector<WaterPoint>(sizeX * sizeY)},
      newBuffer(&points[0]),
      oldBuffer(&points[1]),
      numPasses(8),
      dampening(fixed(4) / fixed(160)),
      tension(fixed(1) / fixed(10)),
      spread(fixed(3) / fixed(4)) {
}

void Water::splash(const Map::Pos &p, float speed) {
    point(p).velocity += speed;  
}

void Water::tick(fixed tickLengthS) {
    for (auto &p : *oldBuffer)
        spring(tickLengthS, p);

    /*fixed rain = fixed(5) / fixed(1);
    point(32,32).velocity += rain;
    point(256-32,256-32).velocity += rain;
    point(256-32,32).velocity += rain;
    point(32,256-32).velocity += rain;*/

    // Point-point interaction
    for (size_t pass = 0; pass < numPasses; pass++) {
        // Copy state to newBuffer
        *newBuffer = *oldBuffer;

        for (size_t x = 0; x < sizeX; x++) {
            for (size_t y = 0; y < sizeY; y++) {
                // Propagate from (x,y) to neighboring points
                if (x > 0) propagate(tickLengthS, x, y, x-1, y);
                if (x < sizeX-1) propagate(tickLengthS, x, y, x+1, y);
                if (y > 0) propagate(tickLengthS, x, y, x, y-1);
                if (y < sizeY-1) propagate(tickLengthS, x, y, x, y+1);

                /*if (x > 0 && y > 0) propagate(tickLengthS, x, y, x-1, y-1);
                if (x < sizeX-1 && y < sizeY-1) propagate(tickLengthS, x, y, x+1, y+1);
                if (x < sizeX-1 && y > 0) propagate(tickLengthS, x, y, x+1, y-1);
                if (x > 0 && y < sizeY-1) propagate(tickLengthS, x, y, x-1, y+1);*/
            }
        }

        std::swap(newBuffer, oldBuffer);
    }
}

void Water::spring(fixed tickLengthS, WaterPoint &point) {
    // Hooke's law with euler integration and dampening
    fixed x = point.height - fixed(100);
    fixed acceleration = -tension * x - dampening * point.velocity;

    point.height += point.velocity * tickLengthS;
    point.velocity += acceleration * tickLengthS;
}

void Water::propagate(fixed tickLengthS, 
                      size_t fromX, size_t fromY,
                      size_t toX, size_t toY) {
    WaterPoint &from(point(fromX, fromY)),
               &oldTo(point(toX, toY)),
               &newTo((*newBuffer)[toY * sizeX + toX]);

    fixed delta = spread * (from.height - oldTo.height); 
    newTo.velocity += delta * tickLengthS;
    //newTo.height += delta * tickLengthS;
}
