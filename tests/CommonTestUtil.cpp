/*
 * CommonTestUtil.cpp
 *
 *  Created on: 2014/06/17
 *      Author: yuuki
 */

#include "tests/CommonTestUtil.h"

namespace ydec {
namespace mahjong {

namespace {
void createToitsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(tsumo ? HandElementType::ANTOITSU : HandElementType::MINTOITSU);
  for (int i = 0; i < 2; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createKantsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(tsumo ? HandElementType::ANKANTSU : HandElementType::MINKANTSU);
  for (int i = 0; i < 4; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createKoutsu(Element* element,
                  const TileType& tile_type,
                  bool tsumo,
                  bool include_agari_hai) {
  element->set_type(tsumo ? HandElementType::ANKOUTSU : HandElementType::MINKOUTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}

void createShuntsu(Element* element,
                   const TileType& smallest_tile_type,
                   bool tsumo,
                   int agari_hai_index) {
  element->set_type(tsumo ? HandElementType::ANSHUNTSU : HandElementType::MINSHUNTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(static_cast<TileType>(smallest_tile_type + i));
    if (i == agari_hai_index) {
      tile->add_state(tsumo ? TileState::AGARI_HAI_TSUMO : TileState::AGARI_HAI_RON);
    }
  }
}
}  // namespace

CommonTestUtil::CommonTestUtil() {}
CommonTestUtil::~CommonTestUtil() {}

Element CommonTestUtil::createAntoitsu(const TileType& tile_type,
                                     bool include_agari_hai) {
  Element element;
  createAntoitsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createAntoitsu(Element* element,
                                  const TileType& tile_type,
                                  bool include_agari_hai) {
  createToitsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::createMintoitsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  createMintoitsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createMintoitsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createToitsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::createAnkantsu(const TileType& tile_type,
                                       bool include_agari_hai) {
  Element element;
  createAnkantsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createAnkantsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  createKantsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::createMinkantsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  createMinkantsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createMinkantsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createKantsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::createAnkoutsu(const TileType& tile_type,
                                       bool include_agari_hai) {
  Element element;
  createAnkoutsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createAnkoutsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  createKoutsu(element, tile_type, true, include_agari_hai);
}

Element CommonTestUtil::createMinkoutsu(const TileType& tile_type,
                                        bool include_agari_hai) {
  Element element;
  createMinkoutsu(&element, tile_type, include_agari_hai);
  return element;
}

void CommonTestUtil::createMinkoutsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  createKoutsu(element, tile_type, false, include_agari_hai);
}

Element CommonTestUtil::createAnshuntsu(const TileType& smallest_tile_type,
                                        int agari_hai_index) {
  Element element;
  createAnshuntsu(&element, smallest_tile_type, agari_hai_index);
  return element;
}

void CommonTestUtil::createAnshuntsu(Element* element,
                                     const TileType& smallest_tile_type,
                                     int agari_hai_index) {
  createShuntsu(element, smallest_tile_type, true, agari_hai_index);
}

Element CommonTestUtil::createMinshuntsu(const TileType& smallest_tile_type,
                                         int agari_hai_index) {
  Element element;
  createMinshuntsu(&element, smallest_tile_type, agari_hai_index);
  return element;
}

void CommonTestUtil::createMinshuntsu(Element* element,
                                      const TileType& smallest_tile_type,
                                      int agari_hai_index) {
  createShuntsu(element, smallest_tile_type, false, agari_hai_index);
}

} /* namespace msc */
} /* namespace ydec */
