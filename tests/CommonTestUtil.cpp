/*
 * CommonTestUtil.cpp
 *
 *  Created on: 2014/06/17
 *      Author: yuuki
 */

#include <tests/CommonTestUtil.h>

using namespace ydec::mahjong;

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

void CommonTestUtil::createAnkou(Element* element,
                                 const TileType& tile_type,
                                 bool include_agari_hai) {
  element->set_type(HandElementType::ANKOUTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(tile_type);
    if (include_agari_hai && i == 0) {
      tile->add_state(TileState::AGARI_HAI_TSUMO);
    }
  }
}

void CommonTestUtil::createAnshuntsu(mahjong::Element* element,
                                     const mahjong::TileType& smallest_tile_type,
                                     int agari_hai_index) {
  element->set_type(HandElementType::ANSHUNTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_type(static_cast<mahjong::TileType>(smallest_tile_type + i));
    if (i == agari_hai_index) {
      tile->add_state(TileState::AGARI_HAI_TSUMO);
    }
  }
}

} /* namespace msc */
} /* namespace ydec */
