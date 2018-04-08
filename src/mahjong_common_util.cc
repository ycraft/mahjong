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

#include "src/mahjong_common_util.h"

#include <algorithm>
#include <string>

#include "src/mahjong_common_value.h"

using std::find_if;
using std::string;

namespace ycraft {
namespace mahjong {

namespace {

bool IsMatched(unsigned int required, unsigned int actual, unsigned int mask) {
  return !(required & mask) || (required & mask) == (actual & mask);
}

bool IsMatched(unsigned int required, unsigned int actual) {
  return IsMatched(required, actual, 0xffffffff);
}

bool IsMatchedForHierarchalData(unsigned int required, unsigned int actual) {
  while (required < actual) {
    actual >>= 4;
  }
  return required == actual;
}

bool ContainsRequiredTileState(const TileState required_state,
                               const Tile& tile) {
  return std::find_if(tile.state().begin(), tile.state().end(),
                      [&required_state](int state) {
                        return IsTileStateMatched(
                            required_state, static_cast<TileState>(state));
                      }) != tile.state().end();
}

}  // namespace

bool IsSequentialTileType(TileType tile) {
  return (tile & MASK_TILE_SEQUENTIAL) == SEQUENTIAL_TILE;
}

bool IsTileTypeMatched(TileType required, TileType tile) {
  return IsTileTypeMatched(required, tile, TileType::MASK_TILE_NUMBER) &&
         IsTileTypeMatched(required, tile, TileType::MASK_TILE_KIND) &&
         IsTileTypeMatched(required, tile, TileType::MASK_TILE_SEQUENTIAL);
}

bool IsTileTypeMatched(TileType required, TileType tile, TileType mask) {
  return IsMatched(required, tile, mask);
}

bool IsTileStateMatched(TileState required, TileState actual) {
  return IsMatchedForHierarchalData(required, actual);
}

bool IsHandElementTypeMatched(HandElementType required,
                              HandElementType element_type) {
  return IsMatchedForHierarchalData(required, element_type);
}

bool IsMachiTypeMatched(MachiType required, MachiType type) {
  return IsMachiTypeMatched(required, type, MachiType::MASK_MACHI_FU) &&
         IsMachiTypeMatched(required, type, MachiType::MASK_MACHI_KIND);
}

bool IsMachiTypeMatched(MachiType required, MachiType type, MachiType mask) {
  return IsMatched(required, type, mask);
}

bool IsAgariTypeMatched(AgariType required, AgariType type) {
  return IsMatched(required, type);
}

bool IsAgariStateMatched(AgariState required, AgariState state) {
  return IsMatched(required, state);
}

bool IsAgariFormatMatched(AgariFormat required, AgariFormat actual) {
  return IsMatched(required, actual);
}

bool IsRichiTypeMatched(RichiType required, RichiType actual) {
  return IsMatchedForHierarchalData(required, actual);
}

bool IsYaochuhai(TileType tile) {
  return !IsSequentialTileType(tile) ||
         IsTileTypeMatched(TileType::TILE_1, tile,
                           TileType::MASK_TILE_NUMBER) ||
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
          contains_agari_tile |= IsTileStateMatched(
              TileState::AGARI_HAI, static_cast<TileState>(state));
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

string GetDebugString(const HandParserResult& result) {
  string str;
  for (const ParsedHand& parsedHand : result.parsed_hand()) {
    if (!(str.empty() || str.back() == '\r' || str.back() == '\n')) {
      str += " ";
    }
    str += (IsAgariFormatMatched(AgariFormat::REGULAR_AGARI,
                                 parsedHand.agari().format()) ||
                    IsAgariFormatMatched(AgariFormat::CHITOITSU_AGARI,
                                         parsedHand.agari().format())
                ? "YES: "
                : " NO: ");
    for (const Element& element : parsedHand.element()) {
      bool is_naki =
          IsHandElementTypeMatched(HandElementType::MINSHUNTSU,
                                   element.type()) ||
          IsHandElementTypeMatched(HandElementType::MINKOUTSU,
                                   element.type()) ||
          IsHandElementTypeMatched(HandElementType::MINKANTSU, element.type());
      str += "{";
      if (is_naki) {
        str += "(";
      }

      string element_tiles;
      for (const Tile& tile : element.tile()) {
        string tile_string = TileType_Name(tile.type());
        if (ContainsRequiredTileState(TileState::AGARI_HAI, tile)) {
          tile_string = "[" + tile_string + "]";
        }
        if (!element_tiles.empty()) {
          element_tiles += " ";
        }
        element_tiles += tile_string;
      }
      str += element_tiles;

      if (is_naki) {
        str += ")";
      }
      str += "}, ";
    }
    str += " " + MachiType_Name(parsedHand.machi_type());
    str += "\n";
  }
  return str;
}

}  // namespace mahjong
}  // namespace ycraft
