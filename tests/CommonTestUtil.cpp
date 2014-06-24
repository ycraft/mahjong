/*
 * CommonTestUtil.cpp
 *
 *  Created on: 2014/06/17
 *      Author: yuuki
 */

#include <tests/CommonTestUtil.h>

using namespace ydec::mahjong;

namespace {
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
}

namespace ydec {
namespace msc {

CommonTestUtil::CommonTestUtil() {}

CommonTestUtil::~CommonTestUtil() {}

void CommonTestUtil::createToitsu(Element* element,
                                  const TileType& tile_type,
                                  bool include_agari_hai) {
  element->set_type(HandElementType::TOITSU);
  for (int i = 0; i < 2; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(TileState::AGARI_HAI_TSUMO);
    }
  }
}

void CommonTestUtil::createAnkantsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  return createKantsu(element, tile_type, true, include_agari_hai);
}

void CommonTestUtil::createAnkoutsu(Element* element,
                                    const TileType& tile_type,
                                    bool include_agari_hai) {
  return createKoutsu(element, tile_type, true, include_agari_hai);
}

void CommonTestUtil::createMinkoutsu(Element* element,
                                     const TileType& tile_type,
                                     bool include_agari_hai) {
  return createKoutsu(element, tile_type, false, include_agari_hai);
}

void CommonTestUtil::createAnshuntsu(Element* element,
                                     const TileType& smallest_tile_type,
                                     int agari_hai_index) {
  return createShuntsu(element, smallest_tile_type, true, agari_hai_index);
}

void CommonTestUtil::createMinshuntsu(Element* element,
                                      const TileType& smallest_tile_type,
                                      int agari_hai_index) {
  return createShuntsu(element, smallest_tile_type, false, agari_hai_index);
}

} /* namespace msc */
} /* namespace ydec */
