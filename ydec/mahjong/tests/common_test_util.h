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

#ifndef YDEC_MAHJONG_TESTS_COMMON_TEST_UTIL_H_
#define YDEC_MAHJONG_TESTS_COMMON_TEST_UTIL_H_

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
  static Element CreateAntoitsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void CreateAntoitsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element CreateMintoitsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void CreateMintoitsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element CreateAnkantsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void CreateAnkantsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element CreateMinkantsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void CreateMinkantsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element CreateAnkoutsu(const TileType& tile_type,
                                bool include_agari_hai = false);
  static void CreateAnkoutsu(Element* element,
                             const TileType& tile_type,
                             bool include_agari_hai = false);

  static Element CreateMinkoutsu(const TileType& tile_type,
                                 bool include_agari_hai = false);
  static void CreateMinkoutsu(Element* element,
                              const TileType& tile_type,
                              bool include_agari_hai = false);

  static Element CreateAnshuntsu(const TileType& smallest_tile_type,
                                 int agari_hai_index = -1);
  static void CreateAnshuntsu(Element* element,
                              const TileType& smallest_tile_type,
                              int agari_hai_index = -1);

  static Element CreateMinshuntsu(const TileType& smallest_tile_type,
                                  int agari_hai_index = -1);
  static void CreateMinshuntsu(Element* element,
                               const TileType& smallest_tile_type,
                               int agari_hai_index = -1);
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_TESTS_COMMON_TEST_UTIL_H_
