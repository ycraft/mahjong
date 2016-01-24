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
namespace mahjong {

class CommonTestUtil {
 private:
  CommonTestUtil();
  ~CommonTestUtil();

 public:
  static Element createAntoitsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void createAntoitsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element createMintoitsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void createMintoitsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element createAnkantsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void createAnkantsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element createMinkantsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void createMinkantsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element createAnkoutsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void createAnkoutsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element createMinkoutsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void createMinkoutsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element createAnshuntsu(const TileType& smallest_tile_type,
                                 int agari_hai_index = -1);
  static void createAnshuntsu(Element* element,
                              const TileType& smallest_tile_type,
                              int agari_hai_index = -1);

  static Element createMinshuntsu(const TileType& smallest_tile_type,
                                  int agari_hai_index = -1);
  static void createMinshuntsu(Element* element,
                               const TileType& smallest_tile_type,
                               int agari_hai_index = -1);
};

} /* namespace mahjong */
} /* namespace ydec */

#endif /* COMMONTESTUTIL_H_ */
