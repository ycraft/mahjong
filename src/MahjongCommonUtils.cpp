#include "MahjongCommonUtils.h"

using namespace ydec::mahjong;

namespace ydec {
namespace msc {

bool MahjongCommonUtils::isSequentialTileType(TileType tile) {
  return (tile & MASK_TILE_SEQUENTIAL) == SEQUENTIAL_TILE;
}


bool MahjongCommonUtils::isTileTypeMatched(TileType required, TileType tile) {
  return isTileTypeMatched(required, tile, TileType::MASK_TILE_NUMBER)
      && isTileTypeMatched(required, tile, TileType::MASK_TILE_KIND)
      && isTileTypeMatched(required, tile, TileType::MASK_TILE_SEQUENTIAL);
}

bool MahjongCommonUtils::isTileTypeMatched(TileType required, TileType tile,
                                           TileType mask) {
  return !(required & mask)
      || (required & mask) == (tile && mask);
}

}
}
