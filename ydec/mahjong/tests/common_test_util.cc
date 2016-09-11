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

#include "ydec/mahjong/tests/common_test_util.h"

namespace ydec {
namespace mahjong {

namespace {
void createToitsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(
      tsumo ? HandElementType::ANTOITSU : HandElementType::MINTOITSU);
  for (int i = 0; i < 2; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(
          tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createKantsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(
      tsumo ? HandElementType::ANKANTSU : HandElementType::MINKANTSU);
  for (int i = 0; i < 4; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(
          tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createKoutsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(
      tsumo ? HandElementType::ANKOUTSU : HandElementType::MINKOUTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(
          tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createShuntsu(Element* element,
                   const TileType& smallest_tile_type,
                   bool tsumo,
                   int agari_hai_index) {
  element->set_type(
      tsumo ? HandElementType::ANSHUNTSU : HandElementType::MINSHUNTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(static_cast<TileType>(smallest_tile_type + i));
    if (i == agari_hai_index) {
      tile->add_state(
          tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}
}  // namespace

CommonTestUtil::CommonTestUtil() {}
CommonTestUtil::~CommonTestUtil() {}

Element CommonTestUtil::CreateAntoitsu(const TileType& tile_type,
                                     bool include_agari_hai) {
  Element element;
  CreateAntoitsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateAntoitsu(Element* element,
                                  const TileType& tile_type,
                                  bool include_agari_hai) {
  createToitsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::CreateMintoitsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  CreateMintoitsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateMintoitsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createToitsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::CreateAnkantsu(const TileType& tile_type,
                                       bool include_agari_hai) {
  Element element;
  CreateAnkantsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateAnkantsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  createKantsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::CreateMinkantsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  CreateMinkantsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateMinkantsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createKantsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::CreateAnkoutsu(const TileType& tile_type,
                                       bool include_agari_hai) {
  Element element;
  CreateAnkoutsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateAnkoutsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  createKoutsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::CreateMinkoutsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  CreateMinkoutsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::CreateMinkoutsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createKoutsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::CreateAnshuntsu(const TileType& smallest_tile_type,
                                        int agari_hai_index) {
  Element element;
  CreateAnshuntsu(&element, smallest_tile_type, agari_hai_index);
  return element;
}

void CommonTestUtil::CreateAnshuntsu(Element* element,
                                     const TileType& smallest_tile_type,
                                     int agari_hai_index) {
  createShuntsu(element, smallest_tile_type, true, agari_hai_index);
}

Element CommonTestUtil::CreateMinshuntsu(const TileType& smallest_tile_type,
                                         int agari_hai_index) {
  Element element;
  CreateMinshuntsu(&element, smallest_tile_type, agari_hai_index);
  return element;
}

void CommonTestUtil::CreateMinshuntsu(Element* element,
                                      const TileType& smallest_tile_type,
                                      int agari_hai_index) {
  createShuntsu(element, smallest_tile_type, false, agari_hai_index);
}

}  // namespace mahjong
}  // namespace ydec
