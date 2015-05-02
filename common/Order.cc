#include "Order.hh"

#include "BitStream.hh"

#include <cassert>

void read(BitStreamReader &reader, Order &order) {
    read(reader, order.player);
    read(reader, order.type);

    switch (order.type) {
        case Order::UNDEFINED:
            assert(false);
            return;
        case Order::ACCELERATE:
            read(reader, order.accelerate.direction);
            return;
    }
}

void write(BitStreamWriter &writer, const Order &order) {
    write(writer, order.player);
    write(writer, order.type);

    switch (order.type) {
        case Order::UNDEFINED:
            assert(false);
            return;
        case Order::ACCELERATE:
            write(writer, order.accelerate.direction);
            return;
    }
}
