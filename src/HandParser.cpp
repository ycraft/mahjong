/*
 * HandParser.cpp
 *
 *  Created on: 2014/02/16
 *      Author: yuuki
 */
#include "src/HandParser.h"

#include <algorithm>
#include <set>

#include "MahjongCommonUtils.h"

using namespace std;
using namespace ydec::mahjong;

namespace ydec {
namespace msc {

string HandParserResultUtil::getDebugString(const HandParserResult& result) {
  string str;
  for (const ParsedHand& parsedHand : result.parsed_hand()) {
    if (!(str.empty() || str.back() == '\r' || str.back() == '\n')) {
      str += " ";
    }
    str += (parsedHand.is_agarikei() ? "YES: " : " NO: ");
    for (const Element& element : parsedHand.element()) {
      for (const Tile &tile : element.tile()) {
        string tile_string = TileType_Name(tile.type());
        if (!tile.is_tsumo()) {
          tile_string = "(" + tile_string + ")";
        }
        if (tile.is_agari_hai()) {
          tile_string = "[" + tile_string + "]";
        }
        str += " " + tile_string;
      }
      str += ",";
    }
    str += "\n";
  }
  return str;
}

HandParser::HandParser() : _hand(nullptr), _num_free_tiles(0), _result(nullptr) {
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
  _num_free_tiles = hand.closed_tile_size() + 1;

  for (int i = 0; i < hand.closed_tile_size(); ++i) {
  _free_tiles[i] = hand.closed_tile(i);
  }
  _free_tiles[_num_free_tiles - 1] = hand.agari_tile();

  sort(_free_tiles, _free_tiles + _num_free_tiles);

  memset(_free_tile_group_ids, 0, _num_free_tiles * sizeof(_free_tile_group_ids[0]));

  _hand = &hand;
  _result = result;
}

void HandParser::runDfs() {
  dfs(0, 1, false);
}

void HandParser::dfs(int i, int id, bool has_jantou) {
  if (i == _num_free_tiles && has_jantou) {
    addAgarikeiResult(id - 1);
    return;
  }

  if (_free_tile_group_ids[i]) {
    dfs(i + 1, id, has_jantou);
    return;
  }

  _free_tile_group_ids[i] = id;

  // anko or jantou
  for (int j = i + 1, counter = 0; j < _num_free_tiles && _free_tiles[i] == _free_tiles[j]; ++j) {
    if (_free_tile_group_ids[j] == 0) {
      ++counter;
      _free_tile_group_ids[j] = id;

      if (!has_jantou && counter == 1) {
        _free_tile_element_types[j] = TOITSU;
        dfs(j + 1, id + 1, true);
      } else if (counter == 2) {
        _free_tile_element_types[j] = KOUTSU;
        dfs(j + 1, id + 1, has_jantou);
        break;
      }
    }
  }

  // Reset state
  for (int j = i + 1; j < _num_free_tiles && _free_tiles[i] == _free_tiles[j]; ++j) {
    if (_free_tile_group_ids[j] == id) {
      _free_tile_group_ids[j] = 0;
    }
  }

  // shuntsu
  if (MahjongCommonUtils::isSequentialTileType(_free_tiles[i])) {
    TileType prev = _free_tiles[i];
    for (int j = i + 1, counter = 0; j < _num_free_tiles && _free_tiles[j] - prev <= 1; ++j) {
      if (_free_tile_group_ids[j] == 0 && _free_tiles[j] - prev == 1) {
        ++counter;
        prev = _free_tiles[j];
        _free_tile_group_ids[j] = id;

        if (counter == 2) {
          _free_tile_element_types[j] = SHUNTSU;
          dfs(i + 1, id + 1, has_jantou);
          break;
        }
      }
    }
  }

  // Reset state
  for (int j = i; j < _num_free_tiles; ++j) {
    if (_free_tile_group_ids[j] == id) {
      _free_tile_group_ids[j] = 0;
    }
  }
}

void HandParser::checkChiiToitsu() {
  if (_hand->chiied_tile_size() != 0
      || _hand->ponned_tile_size() != 0
      || _hand->kanned_tile_size() != 0) {
    return;
  }

  set<TileType> used_tiles;
  for (int i = 1; i < _num_free_tiles; i += 2) {
    if (_free_tiles[i - 1] != _free_tiles[i]) {
      return;
    }
    if (!used_tiles.insert(_free_tiles[i]).second) {
      return;
    }
  }
  if (used_tiles.size() != 7) {
    return;
  }

  for (int i = 1; i < _num_free_tiles; i += 2) {
    _free_tile_group_ids[i - 1] = _free_tile_group_ids[i] = 1 + i / 2;
    _free_tile_element_types[i] = TOITSU;
  }

  addAgarikeiResult(7);

  // reset state
  memset(_free_tile_group_ids, 0, _num_free_tiles * sizeof(_free_tile_group_ids[0]));
  memset(_free_tile_element_types, 0, _num_free_tiles * sizeof(_free_tile_element_types[0]));
}

void HandParser::checkIrregular() {
  if (_num_free_tiles != 14) {
    return;
  }

  ParsedHand* parsed_hand = _result->add_parsed_hand();
  parsed_hand->set_is_agarikei(false);

  Element* element = parsed_hand->add_element();
  element->set_type(HandElementType::IRREGULAR);
  for (const int closed_tile : _hand->closed_tile()) {
    Tile* tile = element->add_tile();
    tile->set_type(static_cast<TileType>(closed_tile));
    tile->set_is_tsumo(true);
    tile->set_is_agari_hai(false);
  }
  Tile* tile = element->add_tile();
  tile->set_type(_hand->agari_tile());
  tile->set_is_tsumo(_hand->agari_type() == AgariType::TSUMO);
  tile->set_is_agari_hai(true);
}

void HandParser::addAgarikeiResult(int last_id) {
  for (int agari_tile_id = 1; agari_tile_id <= last_id; ++agari_tile_id) {
    bool valid_agari_tile_id = false;
    for (int i = 0; i < _num_free_tiles; ++i) {
      if (_free_tile_group_ids[i] == agari_tile_id
          && _free_tiles[i] == _hand->agari_tile()) {
        valid_agari_tile_id = true;
        break;
      }
    }

    if (!valid_agari_tile_id) {
      continue;
    }

    ParsedHand* parsed_hand = _result->add_parsed_hand();
    parsed_hand->set_is_agarikei(true);

    // Parse closed tiles
    for (int id = 1; id <= last_id; ++id) {
      Element* element = parsed_hand->add_element();

      // Set element type
      // Search element type from the last to the first.
      // You need to element type only to the last one for each group.
      for (int i = _num_free_tiles - 1; i >= 0; --i) {
        if (_free_tile_group_ids[i] == id) {
          element->set_type(_free_tile_element_types[i]);
          break;
        }
      }

      // Set tiles
      bool has_set_agari_tile = false;
      for (int i = 0; i < _num_free_tiles; ++i) {
        if (_free_tile_group_ids[i] == id) {
          Tile* tile = element->add_tile();
          tile->set_type(_free_tiles[i]);

          if (!has_set_agari_tile && _free_tiles[i] == _hand->agari_tile() && id == agari_tile_id) {
            has_set_agari_tile = true;
            tile->set_is_tsumo(_hand->agari_type() == AgariType::TSUMO);
            tile->set_is_agari_hai(true);
          } else {
            tile->set_is_tsumo(true);
            tile->set_is_agari_hai(false);
          }
        }
      }
    }

    // Parse Naki tiles
    for (int i = 0; i < _hand->chiied_tile_size(); ++i) {
      Element* element = parsed_hand->add_element();
      element->set_type(SHUNTSU);

      const Hand_Chii& chiied_tile = _hand->chiied_tile(i);
      for (int j = 0; j < chiied_tile.tile_size(); ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(chiied_tile.tile(j));
        tile->set_is_tsumo(false);
        tile->set_is_agari_hai(false);
      }
    }

    for (int i = 0; i < _hand->ponned_tile_size(); ++i) {
      Element* element = parsed_hand->add_element();
      element->set_type(KOUTSU);

      const Hand_Pon& ponned_tile = _hand->ponned_tile(i);
      for (int j = 0; j < 3; ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(ponned_tile.tile());
        tile->set_is_tsumo(false);
        tile->set_is_agari_hai(false);
      }
    }

    for (int i = 0; i < _hand->kanned_tile_size(); ++i) {
      Element* element = parsed_hand->add_element();
      element->set_type(KANTSU);

      const Hand_Kan& kanned_tile = _hand->kanned_tile(i);
      for (int j = 0; j < 4; ++j) {
        Tile* tile = element->add_tile();
        tile->set_type(kanned_tile.tile());
        tile->set_is_tsumo(kanned_tile.is_closed());
        tile->set_is_agari_hai(false);
      }
    }
  }
}

inline bool checkSame(const Tile& lhs, const Tile& rhs) {
  return lhs.type() == rhs.type()
      && lhs.is_tsumo() == rhs.is_tsumo()
      && lhs.is_agari_hai() == rhs.is_agari_hai();
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

  if (lhs.element_size() != rhs.element_size()) {
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
  for (const ParsedHand& parsed_hand : _result->parsed_hand()) {
    bool duplicated = false;
    for (const ParsedHand& entry : dedupedResult.parsed_hand()) {
      duplicated |= checkSame(parsed_hand, entry);
      if (duplicated) break;
    }
    if (!duplicated) dedupedResult.add_parsed_hand()->CopyFrom(parsed_hand);
  }
  _result->Swap(&dedupedResult);
}

} // msc
} // ydec

