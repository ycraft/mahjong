#include "MahjongCommonUtils.h"

using namespace ydec::mahjong;

namespace ydec {
namespace msc {

namespace {
  bool isMatched(unsigned int required, unsigned int actual, unsigned int mask) {
    return !(required & mask)
        || (required & mask) == (actual & mask);
  }
}

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
  return isMatched(required, tile, mask);
}

bool MahjongCommonUtils::isHandElementTypeMatched(HandElementType required,
                                                  HandElementType element_type) {
  unsigned int type = element_type;
  while (required < type) {
    type >>= 4;
  }
  return required == type;
}

bool MahjongCommonUtils::isMachiTypeMatched(MachiType required, MachiType type) {
  return isMachiTypeMatched(required, type, MachiType::MASK_MACHI_FU)
      && isMachiTypeMatched(required, type, MachiType::MASK_MACHI_KIND);
}

bool MahjongCommonUtils::isMachiTypeMatched(MachiType required, MachiType type,
                                            MachiType mask) {
  return isMatched(required, type, mask);
}

}
}
