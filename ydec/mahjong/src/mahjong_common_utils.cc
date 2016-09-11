// Copyright 2016 Yuki Hamada
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

bool IsSequentialTileType(TileType tile) {
  return (tile & MASK_TILE_SEQUENTIAL) == SEQUENTIAL_TILE;
}


bool IsTileTypeMatched(TileType required, TileType tile) {
  return IsTileTypeMatched(required, tile, TileType::MASK_TILE_NUMBER)
      && IsTileTypeMatched(required, tile, TileType::MASK_TILE_KIND)
      && IsTileTypeMatched(required, tile, TileType::MASK_TILE_SEQUENTIAL);
}

bool IsTileTypeMatched(TileType required, TileType tile, TileType mask) {
  return isMatched(required, tile, mask);
}

bool IsTileStateMatched(TileState required, TileState actual) {
  return isMatchedForHierarchalData(required, actual);
}

bool IsHandElementTypeMatched(HandElementType required,
                              HandElementType element_type) {
  return isMatchedForHierarchalData(required, element_type);
}

bool IsMachiTypeMatched(MachiType required, MachiType type) {
  return IsMachiTypeMatched(required, type, MachiType::MASK_MACHI_FU)
      && IsMachiTypeMatched(required, type, MachiType::MASK_MACHI_KIND);
}

bool IsMachiTypeMatched(MachiType required, MachiType type, MachiType mask) {
  return isMatched(required, type, mask);
}

bool IsAgariTypeMatched(AgariType required, AgariType type) {
  return isMatched(required, type);
}

bool IsAgariStateMatched(AgariState required, AgariState state) {
  return isMatched(required, state);
}

bool IsAgariFormatMatched(AgariFormat required, AgariFormat actual) {
  return isMatched(required, actual);
}

bool IsRichiTypeMatched(RichiType required, RichiType actual) {
  return isMatchedForHierarchalData(required, actual);
}

bool IsYaochuhai(TileType tile) {
  return
      !IsSequentialTileType(tile) ||
      IsTileTypeMatched(TileType::TILE_1, tile, TileType::MASK_TILE_NUMBER) ||
      IsTileTypeMatched(TileType::TILE_9, tile, TileType::MASK_TILE_NUMBER);
}

bool IsMenzen(const ParsedHand& hand) {
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
          contains_agari_tile |=
              IsTileStateMatched(TileState::AGARI_HAI,
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
