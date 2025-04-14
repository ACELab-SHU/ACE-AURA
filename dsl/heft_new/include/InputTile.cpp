#include "InputTile.hpp"

std::vector<Tile> InputTile::setupTiles() {
    std::vector<Tile> tiles;
    tiles = {
                {0, 1, 1, 1, false, false, false},
                {1, 1, 1, 1, false, false, false},
                {2, 1, 1, 1, false, false, false}
            };
            
    return tiles;
}