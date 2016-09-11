#include "ydec/mahjong/src/mahjong_common_utils.h"

#include "ydec/mahjong/src/mahjong_common_value.h"

namespace ydec {
namespace mahjong {

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

}  // namespace

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

bool MahjongCommonUtils::isAgariTypeMatched(AgariType required,
                                            AgariType type) {
  return isMatched(required, type);
}

bool MahjongCommonUtils::isAgariStateMatched(AgariState required,
                                             AgariState state) {
  return isMatched(required, state);
}

bool MahjongCommonUtils::isAgariFormatMatched(AgariFormat required,
                                              AgariFormat actual) {
  return isMatched(required, actual);
}

bool MahjongCommonUtils::isRichiTypeMatched(RichiType required,
                                            RichiType actual) {
  return isMatchedForHierarchalData(required, actual);
}

bool MahjongCommonUtils::isYaochuhai(TileType tile) {
  return !isSequentialTileType(tile) ||
         isTileTypeMatched(TileType::TILE_1, tile, TileType::MASK_TILE_NUMBER) ||
         isTileTypeMatched(TileType::TILE_9, tile, TileType::MASK_TILE_NUMBER);
}

bool MahjongCommonUtils::isMenzen(const ParsedHand& hand) {
  for (const Element& element : hand.element()) {
    if (element.type() == HandElementType::MINSHUNTSU ||
        element.type() == HandElementType::MINKOUTSU ||
        element.type() == HandElementType::MINKANTSU) {
      // Check if this mentsu contains agari tile. If it contains agari tile,
      // the hand still can be menzen, because RON doesn't count for naki.
      // (HandParcer annotates mentsu including RON tile MIN*).
      bool contains_agari_tile = false;
      for (const Tile& tile : element.tile()) {
        for (int state : tile.state()) {
          contains_agari_tile |= isTileStateMatched(TileState::AGARI_HAI,
                                                    static_cast<TileState>(state));
          if (contains_agari_tile) {
            break;
          }
        }
        if (contains_agari_tile) {
          break;
        }
      }
      if (!contains_agari_tile) {
        return false;
      }
    }
  }
  return true;
}

}  // namespace mahjong
}  // namespace ydec
