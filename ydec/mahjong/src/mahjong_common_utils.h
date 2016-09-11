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

#ifndef YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTILS_H_
#define YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTILS_H_

#include "ydec/mahjong/proto/mahjong-scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

class MahjongCommonUtils {
 public:
  // Utilities for TileType.
  static bool isSequentialTileType(TileType tile);
  static bool isTileTypeMatched(TileType required, TileType tile);
  static bool isTileTypeMatched(TileType required, TileType tile,
                                TileType mask);

  // Utilities for TileState.
  static bool isTileStateMatched(TileState required,
                                 TileState actual);

  // Utilities for HandElementType
  static bool isHandElementTypeMatched(HandElementType required,
                                       HandElementType element_type);

  // Utilities for MachiType.
  static bool isMachiTypeMatched(MachiType required, MachiType type);
  static bool isMachiTypeMatched(MachiType required, MachiType type,
                                 MachiType mask);

  // Utilities for AgariType.
  static bool isAgariTypeMatched(AgariType required, AgariType type);

  // Utilities for AgariState.
  static bool isAgariStateMatched(AgariState required, AgariState state);

  // Utilities for AgariFormat.
  static bool isAgariFormatMatched(AgariFormat required, AgariFormat actual);

  // Utilities for RichiType.
  static bool isRichiTypeMatched(RichiType required, RichiType actual);

  static bool isYaochuhai(TileType tile);

  static bool isMenzen(const ParsedHand& parsed_hand);
};

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTILS_H_
