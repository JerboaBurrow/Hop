#ifndef TILE_H
#define TILE_H

#include <cstdint>
#include <ostream>

enum class Tile : uint_fast8_t {
    NULL_TILE                   = 255,
    EMPTY                       = 0,
    BOTTOM_LEFT                 = 1,
    BOTTOM_RIGHT                = 2,
    BOTTOM_HALF                 = 3,
    TOP_RIGHT                   = 4,
    BOTTOM_LEFT_AND_TOP_RIGHT   = 5,
    RIGHT_HALF                  = 6,
    EMPTY_TOP_LEFT              = 7,
    TOP_LEFT                    = 8,
    LEFT_HALF                   = 9,
    TOP_LEFT_AND_BOTTOM_RIGHT   = 10,
    EMPTY_TOP_RIGHT             = 11,
    TOP_HALF                    = 12,
    EMPTY_BOTTOM_RIGHT          = 13,
    EMPTY_BOTTOM_LEFT           = 14,
    FULL                        = 15
};

std::ostream & operator<<(std::ostream & os, Tile const & t);

const int MAX_TILE = static_cast<int>(Tile::FULL);

#endif /* TILE_H */
