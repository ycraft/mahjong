#include "MahjongCommonUtils.h"

using namespace ydec::mahjong;

namespace ydec {
namespace msc {

namespace {
  bool isMatched(unsigned int required, unsigned int actual, unsigned int mask) {
    return !(required & mask)
        || (required & mask) == (actual & mask);
  }

  bool isMatched(unsigned int required, unsigned int actual) {
    return isMatched(required, actual, 0xffffffff);
  }

  bool isMatchedForHierarchalData(unsigned int required, unsigned int actual) {
    while (required < actual) {
      actual >>= 4;
    }
    return required == actual;
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

bool MahjongCommonUtils::isTileStateMatched(TileState required, TileState actual) {
  return isMatchedForHierarchalData(required, actual);
}

bool MahjongCommonUtils::isHandElementTypeMatched(HandElementType required,
                                                  HandElementType element_type) {
  return isMatchedForHierarchalData(required, element_type);
}

bool MahjongCommonUtils::isMachiTypeMatched(MachiType required, MachiType type) {
  return isMachiTypeMatched(required, type, MachiType::MASK_MACHI_FU)
      && isMachiTypeMatched(required, type, MachiType::MASK_MACHI_KIND);
}

bool MahjongCommonUtils::isMachiTypeMatched(MachiType required, MachiType type,
                                            MachiType mask) {
  return isMatched(required, type, mask);
}

bool MahjongCommonUtils::isPlayerTypeMatched(PlayerType required,
                                             PlayerType type) {
  return isMatched(required, type);
}

bool MahjongCommonUtils::isAgariTypeMatched(AgariType required,
                                            AgariType type) {
  return isMatched(required, type);
}

bool MahjongCommonUtils::isAgariStateMatched(AgariState required,
                                             AgariState state) {
  return isMatched(required, state);
}

}
}
