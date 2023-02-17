#include <World/tile.h>

std::ostream & operator<<(std::ostream & os, Tile const & t)
{
    int h = static_cast<int>(t);
    os << "Tile: code " << h << " geometry: ";
    switch (t)
    {
        case Tile::EMPTY: os << "EMPTY"; break;
        case Tile::BOTTOM_LEFT: os << "BOTTOM_LEFT"; break;
        case Tile::BOTTOM_RIGHT: os << "BOTTOM_RIGHT"; break;
        case Tile::BOTTOM_HALF: os << "BOTTOM_HALF"; break;
        case Tile::TOP_RIGHT: os << "TOP_RIGHT"; break;
        case Tile::BOTTOM_LEFT_AND_TOP_RIGHT: os << "BOTTOM_LEFT_AND_TOP_RIGHT"; break;
        case Tile::RIGHT_HALF: os << "RIGHT_HALF"; break;
        case Tile::EMPTY_TOP_LEFT: os << "EMPTY_TOP_LEFT"; break;
        case Tile::TOP_LEFT: os << "TOP_LEFT"; break;
        case Tile::LEFT_HALF: os << "LEFT_HALF"; break;
        case Tile::TOP_LEFT_AND_BOTTOM_RIGHT: os << "TOP_LEFT_AND_BOTTOM_RIGHT"; break;
        case Tile::EMPTY_TOP_RIGHT: os << "EMPTY_TOP_RIGHT"; break;
        case Tile::TOP_HALF: os << "TOP_HALF"; break;
        case Tile::EMPTY_BOTTOM_RIGHT: os << "EMPTY_BOTTOM_RIGHT"; break;
        case Tile::EMPTY_BOTTOM_LEFT: os << "EMPTY_BOTTOM_LEFT"; break;
        case Tile::FULL: os << "FULL"; break;
        default: os << "UNKNOWN"; break;
    }
    return os;
}