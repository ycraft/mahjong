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

#include "ydec/mahjong/src/hand_parser.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <set>

#include "ydec/mahjong/src/mahjong_common_utils.h"

namespace ydec {
namespace mahjong {

namespace {
  bool containsRequiredTileState(const TileState required_state,
                                 const Tile& tile) {
    return std::find_if(
        tile.state().begin(),
        tile.state().end(),
        [&required_state](int state) {
          return isTileStateMatched(required_state,
                                    static_cast<TileState>(state));
        }) != tile.state().end();
  }
}  // namespace

std::string HandParserResultUtil::getDebugString(
    const HandParserResult& result) {
  std::string str;
  for (const ParsedHand& parsedHand : result.parsed_hand()) {
    if (!(str.empty() || str.back() == '\r' || str.back() == '\n')) {
      str += " ";
    }
    str += (
        isAgariFormatMatched(
            AgariFormat::REGULAR_AGARI,
            parsedHand.agari().format()) ||
        isAgariFormatMatched(
            AgariFormat::CHITOITSU_AGARI,
            parsedHand.agari().format()) ? "YES: " : " NO: ");
    for (const Element& element : parsedHand.element()) {
      bool is_naki =
          isHandElementTypeMatched(
              HandElementType::MINSHUNTSU, element.type()) ||
          isHandElementTypeMatched(
              HandElementType::MINKOUTSU, element.type()) ||
          isHandElementTypeMatched(
              HandElementType::MINKANTSU, element.type());
      str += "{";
      if (is_naki) {
        str += "(";
      }

      std::string element_tiles;
      for (const Tile &tile : element.tile()) {
        std::string tile_string = TileType_Name(tile.type());
        if (containsRequiredTileState(TileState::AGARI_HAI, tile)) {
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

HandParser::HandParser() : hand_(nullptr), num_free_tiles_(0),
    result_(nullptr) {
}

HandParser::~HandParser() {
}

void HandParser::parse(const Hand& hand, HandParserResult* result) {
  setup(hand, result);
  runDfs();
  checkChiiToitsu();
  checkIrregular();
  deduplicateResult();
}

void HandParser::setup(const Hand& hand, HandParserResult* result) {
  num_free_tiles_ = hand.closed_tile_size() + 1;

  for (int i = 0; i < hand.closed_tile_size(); ++i) {
    free_tiles_[i] = hand.closed_tile(i);
  }
  free_tiles_[num_free_tiles_ - 1] = hand.agari_tile();

  std::sort(free_tiles_, free_tiles_ + num_free_tiles_);

  memset(free_tile_group_ids_, 0,
         num_free_tiles_ * sizeof(free_tile_group_ids_[0]));

  hand_ = &hand;
  result_ = result;
}

void HandParser::runDfs() {
  dfs(0, 1, false);
}

void HandParser::dfs(int i, int id, bool has_jantou) {
  if (i == num_free_tiles_ && has_jantou) {
    addAgarikeiResult(id - 1, AgariFormat::REGULAR_AGARI);
    return;
  }

  if (free_tile_group_ids_[i]) {
    dfs(i + 1, id, has_jantou);
    return;
  }

  free_tile_group_ids_[i] = id;

  // toitsu
  if (!has_jantou) {
    free_tile_element_types_[i] = HandElementType::TOITSU;
    for (int j = i + 1;
         j < num_free_tiles_ && free_tiles_[i] == free_tiles_[j];
         ++j) {
      if (free_tile_group_ids_[j] == 0) {
        free_tile_group_ids_[j] = id;
        free_tile_element_types_[j] = HandElementType::TOITSU;
        dfs(j + 1, id + 1, true);

        // Reset id.
        free_tile_group_ids_[j] = 0;
        break;
      }
    }
  }

  // koutsu
  {
    int counter = 0;
    int using_tile_indices[2];
    free_tile_element_types_[i] = HandElementType::KOUTSU;
    for (int j = i + 1;
         j < num_free_tiles_ && free_tiles_[i] == free_tiles_[j];
         ++j) {
      if (free_tile_group_ids_[j] == 0) {
        using_tile_indices[counter] = j;
        ++counter;

        free_tile_group_ids_[j] = id;
        free_tile_element_types_[j] = HandElementType::KOUTSU;

        if (counter == 2) {
          dfs(j + 1, id + 1, has_jantou);
          break;
        }
      }
    }

    // Reset IDs.
    for (int j = 0; j < counter; ++j) {
      free_tile_group_ids_[using_tile_indices[j]] = 0;
    }
  }

  // shuntsu
  if (isSequentialTileType(free_tiles_[i])) {
    int counter = 0;
    int using_tile_indices[2];
    free_tile_element_types_[i] = HandElementType::SHUNTSU;
    TileType prev = free_tiles_[i];
    for (int j = i + 1;
         j < num_free_tiles_ && free_tiles_[j] - prev <= 1;
         ++j) {
      if (free_tile_group_ids_[j] == 0 && free_tiles_[j] - prev == 1) {
        using_tile_indices[counter] = j;
        ++counter;

        prev = free_tiles_[j];
        free_tile_group_ids_[j] = id;
        free_tile_element_types_[j] = HandElementType::SHUNTSU;

        if (counter == 2) {
          dfs(i + 1, id + 1, has_jantou);
          break;
        }
      }
    }

    // Reset IDs.
    for (int j = 0; j < counter; ++j) {
      free_tile_group_ids_[using_tile_indices[j]] = 0;
    }
  }

  // Reset ID.
  free_tile_group_ids_[i] = 0;
}

void HandParser::checkChiiToitsu() {
  if (hand_->chiied_tile_size() != 0
      || hand_->ponned_tile_size() != 0
      || hand_->kanned_tile_size() != 0) {
    return;
  }

  std::set<TileType> used_tiles;
  for (int i = 1; i < num_free_tiles_; i += 2) {
    if (free_tiles_[i - 1] != free_tiles_[i]) {
      return;
    }
    if (!used_tiles.insert(free_tiles_[i]).second) {
      return;
    }
  }
  if (used_tiles.size() != 7) {
    return;
  }

  for (int i = 1; i < num_free_tiles_; i += 2) {
    free_tile_group_ids_[i - 1] = free_tile_group_ids_[i] = 1 + i / 2;
    free_tile_element_types_[i - 1] = free_tile_element_types_[i] =
        HandElementType::TOITSU;
  }

  addAgarikeiResult(7, AgariFormat::CHITOITSU_AGARI);

  // Reset IDs.
  memset(free_tile_group_ids_, 0,
         num_free_tiles_ * sizeof(free_tile_group_ids_[0]));
}

void HandParser::checkIrregular() {
  if (num_free_tiles_ != 14) {
    return;
  }

  ParsedHand* parsed_hand = result_->add_parsed_hand();
  *parsed_hand->mutable_agari() = hand_->agari();
  parsed_hand->mutable_agari()->set_format(AgariFormat::IRREGULAR_AGARI);

  Element* element = parsed_hand->add_element();
  element->set_type(HandElementType::UNKNOWN_HAND_ELEMENT_TYPE);
  for (const int closed_tile : hand_->closed_tile()) {
    Tile* tile = element->add_tile();
    tile->set_type(static_cast<TileType>(closed_tile));
  }
  Tile* tile = element->add_tile();
  tile->set_type(hand_->agari_tile());
  tile->add_state(
      (hand_->agari().type() == AgariType::TSUMO)
      ? TileState::AGARI_HAI_TSUMO
      : TileState::AGARI_HAI_RON);
}

void HandParser::addAgarikeiResult(int last_id, const AgariFormat& format) {
  for (int agari_group_id = 1; agari_group_id <= last_id; ++agari_group_id) {
    bool valid_agari_group_id = false;
    for (int i = 0; i < num_free_tiles_; ++i) {
      if (free_tile_group_ids_[i] == agari_group_id
          && free_tiles_[i] == hand_->agari_tile()) {
        valid_agari_group_id = true;
        break;
      }
    }

    if (!valid_agari_group_id) {
      continue;
    }

    ParsedHand* parsed_hand = result_->add_parsed_hand();

    *parsed_hand->mutable_agari() = hand_->agari();
    parsed_hand->mutable_agari()->set_format(format);

    // Parse closed tiles
    for (int id = 1; id <= last_id; ++id) {
      Element* element = parsed_hand->add_element();

      // Set element type.
      for (int i = 0; i < num_free_tiles_; ++i) {
        if (free_tile_group_ids_[i] == id) {
          bool contains_ron_hai =
              (id == agari_group_id
               && isAgariTypeMatched(
                   AgariType::RON, hand_->agari().type()));
          if (isHandElementTypeMatched(
              HandElementType::TOITSU, free_tile_element_types_[i])) {
            element->set_type(
                contains_ron_hai ?
                    HandElementType::MINTOITSU :
                    HandElementType::ANTOITSU);
          } else if (isHandElementTypeMatched(
              HandElementType::KOUTSU, free_tile_element_types_[i])) {
            element->set_type(
                contains_ron_hai ?
                    HandElementType::MINKOUTSU :
                    HandElementType::ANKOUTSU);
          } else if (isHandElementTypeMatched(
              HandElementType::SHUNTSU, free_tile_element_types_[i])) {
            element->set_type(
                contains_ron_hai ?
                    HandElementType::MINSHUNTSU :
                    HandElementType::ANSHUNTSU);
          } else {
            std::cerr << "Unexpected HandElementType: "
                      << HandElementType_Name(free_tile_element_types_[i])
                      << std::endl;
          }
          break;
        }
      }

      // Set tiles
      bool has_set_agari_tile = false;
      for (int i = 0; i < num_free_tiles_; ++i) {
        if (free_tile_group_ids_[i] == id) {
          Tile* tile = element->add_tile();
          tile->set_type(free_tiles_[i]);

          if (!has_set_agari_tile
              && free_tiles_[i] == hand_->agari_tile()
              && id == agari_group_id) {
            has_set_agari_tile = true;
            tile->add_state(
                (hand_->agari().type() == AgariType::TSUMO)
                ? TileState::AGARI_HAI_TSUMO
                : TileState::AGARI_HAI_RON);

            // Set matchi type.
            if (isHandElementTypeMatched(
                HandElementType::TOITSU, free_tile_element_types_[i])) {
              parsed_hand->set_machi_type(MachiType::TANKI);
            } else if (isHandElementTypeMatched(
                HandElementType::KOUTSU, free_tile_element_types_[i])) {
              parsed_hand->set_machi_type(MachiType::SHABO);
            } else if (isHandElementTypeMatched(
                HandElementType::SHUNTSU, free_tile_element_types_[i])) {
              // free_tiles_[] is sorted in ascendant order and we add tiles
              // into element in the same manner. Thus, if the current element
              // tile size is 2, that means this tile is the middle tile of
              // shuntsu.
              if (element->tile_size() == 2) {
                parsed_hand->set_machi_type(MachiType::KANCHAN);
              } else {
                if (isTileTypeMatched(TileType::TILE_3, free_tiles_[i]) ||
                    isTileTypeMatched(TileType::TILE_7, free_tiles_[i])) {
                  parsed_hand->set_machi_type(MachiType::PENCHAN);
                } else {
                  parsed_hand->set_machi_type(MachiType::RYANMEN);
                }
              }
            } else {
              std::cerr << "Unexpected hand element type: "
                        << free_tile_element_types_[i]
                        << std::endl;
            }
          }
        }
      }
    }

    // Parse Naki tiles
    for (int i = 0; i < hand_->chiied_tile_size(); ++i) {
      const Hand_Chii& chiied_tile = hand_->chiied_tile(i);

      Element* element = parsed_hand->add_element();
      element->set_type(HandElementType::MINSHUNTSU);
      for (int j = 0; j < chiied_tile.tile_size(); ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(chiied_tile.tile(j));
      }
    }

    for (int i = 0; i < hand_->ponned_tile_size(); ++i) {
      const Hand_Pon& ponned_tile = hand_->ponned_tile(i);

      Element* element = parsed_hand->add_element();
      element->set_type(HandElementType::MINKOUTSU);
      for (int j = 0; j < 3; ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(ponned_tile.tile());
      }
    }

    for (int i = 0; i < hand_->kanned_tile_size(); ++i) {
      const Hand_Kan& kanned_tile = hand_->kanned_tile(i);

      Element* element = parsed_hand->add_element();
      element->set_type(
          kanned_tile.is_closed()
          ? HandElementType::ANKANTSU
          : HandElementType::MINKANTSU);
      for (int j = 0; j < 4; ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(kanned_tile.tile());
      }
    }
  }
}

inline bool checkSame(const Tile& lhs, const Tile& rhs) {
  if (lhs.type() != rhs.type() || lhs.state_size() != rhs.state_size()) {
    return false;
  }

  // This equality check is designed by assuming that the given state does not
  // contain duplicated objects.
  for (const int state : lhs.state()) {
    if (std::find(rhs.state().begin(),
                  rhs.state().end(),
                  static_cast<TileState>(state)) == rhs.state().end()) {
      return false;
    }
  }

  return true;
}

inline bool checkSame(const Element& lhs, const Element& rhs) {
  static bool is_used[10];

  if (lhs.type() != rhs.type()) return false;
  if (lhs.tile_size() != rhs.tile_size()) return false;

  memset(is_used, 0, sizeof(is_used));
  for (const Tile& tile : lhs.tile()) {
    bool found = false;
    for (int i = 0; i < rhs.tile_size(); ++i) {
      if (is_used[i]) continue;
      if (!checkSame(tile, rhs.tile(i))) continue;
      is_used[i] = true;
      found = true;
      break;
    }
    if (!found) return false;
  }

  return true;
}

inline bool checkSame(const ParsedHand& lhs, const ParsedHand& rhs) {
  static bool is_used[10];

  if (lhs.element_size() != rhs.element_size()
      || lhs.agari().format() != rhs.agari().format()
      || lhs.machi_type() != rhs.machi_type()) {
    return false;
  }

  memset(is_used, 0, sizeof(is_used));
  for (const Element& element : lhs.element()) {
    bool found = false;
    for (int i = 0; i < rhs.element_size(); ++i) {
      if (is_used[i]) continue;
      if (!checkSame(element, rhs.element(i))) continue;
      is_used[i] = true;
      found = true;
      break;
    }
    if (!found) return false;
  }

  return true;
}

void HandParser::deduplicateResult() {
  HandParserResult dedupedResult;
  for (const ParsedHand& parsed_hand : result_->parsed_hand()) {
    bool duplicated = false;
    for (const ParsedHand& entry : dedupedResult.parsed_hand()) {
      duplicated |= checkSame(parsed_hand, entry);
      if (duplicated) break;
    }
    if (!duplicated) dedupedResult.add_parsed_hand()->CopyFrom(parsed_hand);
  }
  result_->Swap(&dedupedResult);
}

}  // namespace mahjong
}  // namespace ydec
