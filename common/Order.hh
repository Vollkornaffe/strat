#ifndef STRAT_COMMON_ORDER_HH
#define STRAT_COMMON_ORDER_HH

#include "Defs.hh"

struct BitStreamReader;
struct BitStreamWriter;

enum Direction {
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_FORWARD,
    DIRECTION_BACKWARD
};

struct Order {
    enum Type {
        UNDEFINED,
        ACCELERATE
    } type;

    Order(Type type = UNDEFINED)
        : type(type) {
    }

    PlayerId player;

    union {
        struct {
            Direction direction;
        } accelerate;
    };
};

void read(BitStreamReader &, Order &);
void write(BitStreamWriter &, const Order &);

#endif
