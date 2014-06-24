/*
 * CommonTestUtil.h
 *
 *  Created on: 2014/06/17
 *      Author: yuuki
 */

#ifndef COMMONTESTUTIL_H_
#define COMMONTESTUTIL_H_

#include "src-gen/cpp/mahjong-common.pb.h"
#include "src-gen/cpp/mahjong-rule.pb.h"
#include "src-gen/cpp/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace msc {

class CommonTestUtil {
 private:
  CommonTestUtil();
  ~CommonTestUtil();

 public:
  static void createToitsu(mahjong::Element* element,
                           const mahjong::TileType& tile_type,
                           bool include_agari_hai = false);

  static void createAnkantsu(mahjong::Element* element,
                             const mahjong::TileType& tile_type,
                             bool include_agari_hai = false);

  static void createAnkoutsu(mahjong::Element* element,
                             const mahjong::TileType& tile_type,
                             bool include_agari_hai = false);
  static void createMinkoutsu(mahjong::Element* element,
                              const mahjong::TileType& tile_type,
                              bool include_agari_hai = false);

  static void createAnshuntsu(mahjong::Element* element,
                              const mahjong::TileType& smallest_tile_type,
                              int agari_hai_index = -1);
  static void createMinshuntsu(mahjong::Element* element,
                               const mahjong::TileType& smallest_tile_type,
                               int agari_hai_index = -1);
};

} /* namespace msc */
} /* namespace ydec */

#endif /* COMMONTESTUTIL_H_ */
