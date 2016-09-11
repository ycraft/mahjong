#ifndef YDEC_MAHJONG_COMMON_TEST_UTIL_H_
#define YDEC_MAHJONG_COMMON_TEST_UTIL_H_

#include "ydec/mahjong/proto/mahjong-common.pb.h"
#include "ydec/mahjong/proto/mahjong-rule.pb.h"
#include "ydec/mahjong/proto/mahjong-scorecalculator.pb.h"

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

}  // namespace ydec
}  // namespace mahjong

#endif  // YDEC_MAHJONG_COMMON_TEST_UTIL_H_
