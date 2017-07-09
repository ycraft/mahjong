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

#ifndef YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTIL_H_
#define YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTIL_H_

#include <string>

#include "ydec/mahjong/proto/mahjong_scorecalculator.pb.h"

namespace ydec {
namespace mahjong {

// Utilities for TileType.
bool IsSequentialTileType(TileType tile);
bool IsTileTypeMatched(TileType required, TileType tile);
bool IsTileTypeMatched(TileType required, TileType tile, TileType mask);

// Utilities for TileState.
bool IsTileStateMatched(TileState required, TileState actual);

// Utilities for HandElementType
bool IsHandElementTypeMatched(HandElementType required,
                              HandElementType element_type);

// Utilities for MachiType.
bool IsMachiTypeMatched(MachiType required, MachiType type);
bool IsMachiTypeMatched(MachiType required, MachiType type, MachiType mask);

// Utilities for AgariType.
bool IsAgariTypeMatched(AgariType required, AgariType type);

// Utilities for AgariState.
bool IsAgariStateMatched(AgariState required, AgariState state);

// Utilities for AgariFormat.
bool IsAgariFormatMatched(AgariFormat required, AgariFormat actual);

// Utilities for RichiType.
bool IsRichiTypeMatched(RichiType required, RichiType actual);

bool IsYaochuhai(TileType tile);

bool IsMenzen(const ParsedHand& parsed_hand);

std::string GetDebugString(const HandParserResult& result);

}  // namespace mahjong
}  // namespace ydec

#endif  // YDEC_MAHJONG_SRC_MAHJONG_COMMON_UTIL_H_
