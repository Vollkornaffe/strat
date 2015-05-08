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


WaterPoint Water::fpoint(const fvec2 &p) const {
    assert(p.x >= 0 && p.x < sizeX);
    assert(p.y >= 0 && p.y < sizeY);

    size_t x1 = p.x.toInt(),
           x2 = x1 < sizeX-1 ? x1 + 1 : x1;
    size_t y1 = p.y.toInt(),
           y2 = y1 < sizeY-1 ? y1 + 1 : y1;

    fixed s = p.x - p.x.toInt(),
          t = p.y - p.y.toInt();

    fixed h11(point(x1, y1).height),
          h21(point(x2, y1).height),
          h22(point(x2, y2).height),
          h12(point(x1, y2).height);
    fixed v11(point(x1, y1).velocity),
          v21(point(x2, y1).velocity),
          v22(point(x2, y2).velocity),
          v12(point(x1, y2).velocity);
    fixed a11(point(x1, y1).acceleration),
          a21(point(x2, y1).acceleration),
          a22(point(x2, y2).acceleration),
          a12(point(x1, y2).acceleration);

    if (s + t <= 1) {
        //std::cout << p.x << "|" << p.y << " (" << s << "|" << t << "): " <<h11 << "," << h21 << "," << h22 << "," << h12 << " -> " << h11 + s * (h21 - h11) + t * (h12 - h11) << std::endl;
        return WaterPoint(h11 + s * (h21 - h11) + t * (h12 - h11),
                          v11 + s * (v21 - v11) + t * (v12 - v11),
                          a11 + s * (a21 - a11) + t * (a12 - a11));
    } else {
        //std::cout << p.x << "|" << p.y << " (" << s << "|" << t << "): " <<h11 << "," << h21 << "," << h22 << "," << h12 << " => " << h22 + (1-s) * (h12 - h22) + (1-t) * (h21 - h22) << std::endl;
        return WaterPoint(h22 + (1-s) * (h12 - h22) + (1-t) * (h21 - h22),
                          v22 + (1-s) * (v12 - v22) + (1-t) * (v21 - v22),
                          a22 + (1-s) * (a12 - a22) + (1-t) * (a21 - a22));
    }
}

void Water::splash(const Map::Pos &p, fixed speed) {
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

    point.acceleration = -tension * x - dampening * point.velocity;
    point.height += point.velocity * tickLengthS;
    point.velocity += point.acceleration * tickLengthS;
}

void Water::propagate(fixed tickLengthS, 
                      size_t fromX, size_t fromY,
                      size_t toX, size_t toY) {
    WaterPoint &from(point(fromX, fromY)),
               &oldTo(point(toX, toY)),
               &newTo((*newBuffer)[toY * sizeX + toX]);

    fixed delta = spread * (from.height - oldTo.height); 
    newTo.velocity += delta * tickLengthS;
    newTo.acceleration += delta * tickLengthS;
    //newTo.height += delta * tickLengthS;
}
