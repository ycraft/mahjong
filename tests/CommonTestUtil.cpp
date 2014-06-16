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
    tile->set_is_agari_hai(include_agari_hai && i == 0);
    tile->set_is_tsumo(true);
    tile->set_type(tile_type);
  }
}

void CommonTestUtil::createAnkou(Element* element,
                                 const TileType& tile_type,
                                 bool include_agari_hai) {
  element->set_type(HandElementType::KOUTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_is_agari_hai(include_agari_hai && i == 0);
    tile->set_is_tsumo(true);
    tile->set_type(tile_type);
  }
}

void CommonTestUtil::createAnshuntsu(mahjong::Element* element,
                                     const mahjong::TileType& smallest_tile_type,
                                     int agari_hai_index) {
  element->set_type(HandElementType::SHUNTSU);
  for (int i = 0; i < 3; ++i) {
    Tile* tile = element->add_tile();
    tile->set_is_agari_hai(agari_hai_index == i);
    tile->set_is_tsumo(true);
    tile->set_type(static_cast<mahjong::TileType>(smallest_tile_type + i));
  }
}

} /* namespace msc */
} /* namespace ydec */
